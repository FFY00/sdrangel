///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 Edouard Griffiths, F4EXB.                                  //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

#include "amdemod.h"

#include <QTime>
#include <QDebug>

#include <stdio.h>
#include <complex.h>

#include "dsp/downchannelizer.h"
#include "audio/audiooutput.h"
#include "dsp/dspengine.h"
#include "dsp/pidcontroller.h"
#include "dsp/threadedbasebandsamplesink.h"
#include "device/devicesourceapi.h"

MESSAGE_CLASS_DEFINITION(AMDemod::MsgConfigureAMDemod, Message)
MESSAGE_CLASS_DEFINITION(AMDemod::MsgConfigureChannelizer, Message)

const QString AMDemod::m_channelID = "de.maintech.sdrangelove.channel.am";
const int AMDemod::m_udpBlockSize = 512;

AMDemod::AMDemod(DeviceSourceAPI *deviceAPI) :
    m_deviceAPI(deviceAPI),
    m_absoluteFrequencyOffset(0),
    m_squelchOpen(false),
	m_magsqSum(0.0f),
	m_magsqPeak(0.0f),
	m_magsqCount(0),
	m_movingAverage(40, 0),
    m_volumeAGC(1200, 1.0),
    m_audioFifo(48000),
    m_settingsMutex(QMutex::Recursive)
{
    setObjectName("AMDemod");

	m_audioBuffer.resize(1<<14);
	m_audioBufferFill = 0;

	m_movingAverage.resize(16, 0);
	m_volumeAGC.resize(4096, 0.003, 0);
	m_magsq = 0.0;

	DSPEngine::instance()->addAudioSink(&m_audioFifo);
    m_udpBufferAudio = new UDPSink<qint16>(this, m_udpBlockSize, m_settings.m_udpPort);

    m_channelizer = new DownChannelizer(this);
    m_threadedChannelizer = new ThreadedBasebandSampleSink(m_channelizer, this);
    m_deviceAPI->addThreadedSink(m_threadedChannelizer);
    m_deviceAPI->addChannelAPI(this);

    applySettings(m_settings, true);
}

AMDemod::~AMDemod()
{
	DSPEngine::instance()->removeAudioSink(&m_audioFifo);
    delete m_udpBufferAudio;
    m_deviceAPI->removeChannelAPI(this);
    m_deviceAPI->removeThreadedSink(m_threadedChannelizer);
    delete m_threadedChannelizer;
    delete m_channelizer;
}

void AMDemod::feed(const SampleVector::const_iterator& begin, const SampleVector::const_iterator& end, bool firstOfBurst __attribute__((unused)))
{
	Complex ci;

	m_settingsMutex.lock();

	for (SampleVector::const_iterator it = begin; it != end; ++it)
	{
		//Complex c(it->real() / 32768.0, it->imag() / 32768.0);
		Complex c(it->real(), it->imag());
		c *= m_nco.nextIQ();

		if (m_interpolatorDistance < 1.0f) // interpolate
		{
            processOneSample(ci);

		    while (m_interpolator.interpolate(&m_interpolatorDistanceRemain, c, &ci))
            {
                processOneSample(ci);
            }

            m_interpolatorDistanceRemain += m_interpolatorDistance;
		}
		else // decimate
		{
	        if (m_interpolator.decimate(&m_interpolatorDistanceRemain, c, &ci))
	        {
	            processOneSample(ci);
	            m_interpolatorDistanceRemain += m_interpolatorDistance;
	        }
		}
	}

	if (m_audioBufferFill > 0)
	{
		uint res = m_audioFifo.write((const quint8*)&m_audioBuffer[0], m_audioBufferFill, 10);

		if (res != m_audioBufferFill)
		{
			qDebug("AMDemod::feed: %u/%u tail samples written", res, m_audioBufferFill);
		}

		m_audioBufferFill = 0;
	}

	m_settingsMutex.unlock();
}

void AMDemod::start()
{
	qDebug() << "AMDemod::start: m_inputSampleRate: " << m_settings.m_inputSampleRate
			<< " m_inputFrequencyOffset: " << m_settings.m_inputFrequencyOffset;

	m_squelchCount = 0;
	m_audioFifo.clear();
}

void AMDemod::stop()
{
}

bool AMDemod::handleMessage(const Message& cmd)
{
	if (DownChannelizer::MsgChannelizerNotification::match(cmd))
	{
		DownChannelizer::MsgChannelizerNotification& notif = (DownChannelizer::MsgChannelizerNotification&) cmd;

		AMDemodSettings settings = m_settings;

		settings.m_inputSampleRate = notif.getSampleRate();
		settings.m_inputFrequencyOffset = notif.getFrequencyOffset();

        applySettings(settings);

        qDebug() << "AMDemod::handleMessage: MsgChannelizerNotification:"
                << " m_inputSampleRate: " << settings.m_inputSampleRate
                << " m_inputFrequencyOffset: " << settings.m_inputFrequencyOffset;

		return true;
	}
	else if (MsgConfigureChannelizer::match(cmd))
	{
	    MsgConfigureChannelizer& cfg = (MsgConfigureChannelizer&) cmd;

        m_channelizer->configure(m_channelizer->getInputMessageQueue(),
            cfg.getSampleRate(),
            cfg.getCenterFrequency());

        qDebug() << "AMDemod::handleMessage: MsgConfigureChannelizer:"
                << " sampleRate: " << cfg.getSampleRate()
                << " inputFrequencyOffset: " << cfg.getCenterFrequency();

        return true;
	}
	else if (MsgConfigureAMDemod::match(cmd))
	{
        MsgConfigureAMDemod& cfg = (MsgConfigureAMDemod&) cmd;

        AMDemodSettings settings = cfg.getSettings();

        // These settings are set with DownChannelizer::MsgChannelizerNotification
        m_absoluteFrequencyOffset = settings.m_inputFrequencyOffset;
	    settings.m_inputSampleRate = m_settings.m_inputSampleRate;
	    settings.m_inputFrequencyOffset = m_settings.m_inputFrequencyOffset;

        applySettings(settings, cfg.getForce());

        qDebug() << "AMDemod::handleMessage: MsgConfigureAMDemod:"
                << " m_rfBandwidth: " << settings.m_rfBandwidth
                << " m_volume: " << settings.m_volume
                << " m_squelch: " << settings.m_squelch
                << " m_audioMute: " << settings.m_audioMute
                << " m_bandpassEnable: " << settings.m_bandpassEnable
                << " m_copyAudioToUDP: " << settings.m_copyAudioToUDP
                << " m_udpAddress: " << settings.m_udpAddress
                << " m_udpPort: " << settings.m_udpPort
                << " force: " << cfg.getForce();

		return true;
	}
	else
	{
		return false;
	}
}

void AMDemod::applySettings(const AMDemodSettings& settings, bool force)
{
    if ((m_settings.m_inputFrequencyOffset != settings.m_inputFrequencyOffset) ||
        (m_settings.m_inputSampleRate != settings.m_inputSampleRate) || force)
    {
        m_nco.setFreq(-settings.m_inputFrequencyOffset, settings.m_inputSampleRate);
    }

    if((m_settings.m_inputSampleRate != settings.m_inputSampleRate) ||
        (m_settings.m_rfBandwidth != settings.m_rfBandwidth) ||
        (m_settings.m_audioSampleRate != settings.m_audioSampleRate) ||
        (m_settings.m_bandpassEnable != settings.m_bandpassEnable) || force)
    {
        m_settingsMutex.lock();
        m_interpolator.create(16, settings.m_inputSampleRate, settings.m_rfBandwidth / 2.2f);
        m_interpolatorDistanceRemain = 0;
        m_interpolatorDistance = (Real) settings.m_inputSampleRate / (Real) settings.m_audioSampleRate;
        m_bandpass.create(301, settings.m_audioSampleRate, 300.0, settings.m_rfBandwidth / 2.0f);
        m_settingsMutex.unlock();
    }

    if ((m_settings.m_squelch != settings.m_squelch) || force)
    {
        m_squelchLevel = pow(10.0, settings.m_squelch / 10.0);
    }

    if ((m_settings.m_udpAddress != settings.m_udpAddress)
        || (m_settings.m_udpPort != settings.m_udpPort) || force)
    {
        m_udpBufferAudio->setAddress(const_cast<QString&>(settings.m_udpAddress));
        m_udpBufferAudio->setPort(settings.m_udpPort);
    }

    m_settings = settings;
}
