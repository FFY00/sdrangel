///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 Edouard Griffiths, F4EXB                                   //
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

#ifndef INCLUDE_SDRDAEMONSOURCEINPUT_H
#define INCLUDE_SDRDAEMONSOURCEINPUT_H

#include <dsp/devicesamplesource.h>
#include <QString>
#include <QTimer>
#include <ctime>
#include <iostream>
#include <stdint.h>

#include "sdrdaemonsourcesettings.h"

class DeviceSourceAPI;
class SDRdaemonSourceUDPHandler;
class FileRecord;

class SDRdaemonSourceInput : public DeviceSampleSource {
public:
    class MsgConfigureSDRdaemonSource : public Message {
        MESSAGE_CLASS_DECLARATION

    public:
        const SDRdaemonSourceSettings& getSettings() const { return m_settings; }
        bool getForce() const { return m_force; }

        static MsgConfigureSDRdaemonSource* create(const SDRdaemonSourceSettings& settings, bool force = false)
        {
            return new MsgConfigureSDRdaemonSource(settings, force);
        }

    private:
        SDRdaemonSourceSettings m_settings;
        bool m_force;

        MsgConfigureSDRdaemonSource(const SDRdaemonSourceSettings& settings, bool force) :
            Message(),
            m_settings(settings),
            m_force(force)
        { }
    };

	class MsgConfigureSDRdaemonUDPLink : public Message {
		MESSAGE_CLASS_DECLARATION

	public:
		const QString& getAddress() const { return m_address; }
		quint16 getPort() const { return m_port; }

		static MsgConfigureSDRdaemonUDPLink* create(const QString& address, quint16 port)
		{
			return new MsgConfigureSDRdaemonUDPLink(address, port);
		}

	private:
		QString m_address;
		quint16 m_port;

		MsgConfigureSDRdaemonUDPLink(const QString& address, quint16 port) :
			Message(),
			m_address(address),
			m_port(port)
		{ }
	};

	class MsgConfigureSDRdaemonAutoCorr : public Message {
		MESSAGE_CLASS_DECLARATION
	public:
		bool getDCBlock() const { return m_dcBlock; }
		bool getIQImbalance() const { return m_iqCorrection; }

		static MsgConfigureSDRdaemonAutoCorr* create(bool dcBlock, bool iqImbalance)
		{
			return new MsgConfigureSDRdaemonAutoCorr(dcBlock, iqImbalance);
		}

	private:
		bool m_dcBlock;
		bool m_iqCorrection;

		MsgConfigureSDRdaemonAutoCorr(bool dcBlock, bool iqImbalance) :
			Message(),
			m_dcBlock(dcBlock),
			m_iqCorrection(iqImbalance)
		{ }
	};

	class MsgConfigureSDRdaemonWork : public Message {
		MESSAGE_CLASS_DECLARATION

	public:
		bool isWorking() const { return m_working; }

		static MsgConfigureSDRdaemonWork* create(bool working)
		{
			return new MsgConfigureSDRdaemonWork(working);
		}

	private:
		bool m_working;

		MsgConfigureSDRdaemonWork(bool working) :
			Message(),
			m_working(working)
		{ }
	};

	class MsgConfigureSDRdaemonStreamTiming : public Message {
		MESSAGE_CLASS_DECLARATION

	public:

		static MsgConfigureSDRdaemonStreamTiming* create()
		{
			return new MsgConfigureSDRdaemonStreamTiming();
		}

	private:

		MsgConfigureSDRdaemonStreamTiming() :
			Message()
		{ }
	};

	class MsgReportSDRdaemonAcquisition : public Message {
		MESSAGE_CLASS_DECLARATION

	public:
		bool getAcquisition() const { return m_acquisition; }

		static MsgReportSDRdaemonAcquisition* create(bool acquisition)
		{
			return new MsgReportSDRdaemonAcquisition(acquisition);
		}

	protected:
		bool m_acquisition;

		MsgReportSDRdaemonAcquisition(bool acquisition) :
			Message(),
			m_acquisition(acquisition)
		{ }
	};

	class MsgReportSDRdaemonSourceStreamData : public Message {
		MESSAGE_CLASS_DECLARATION

	public:
		int getSampleRate() const { return m_sampleRate; }
		quint64 getCenterFrequency() const { return m_centerFrequency; }
		uint32_t get_tv_sec() const { return m_tv_sec; }
		uint32_t get_tv_usec() const { return m_tv_usec; }

		static MsgReportSDRdaemonSourceStreamData* create(int sampleRate, quint64 centerFrequency, uint32_t tv_sec, uint32_t tv_usec)
		{
			return new MsgReportSDRdaemonSourceStreamData(sampleRate, centerFrequency, tv_sec, tv_usec);
		}

	protected:
		int m_sampleRate;
		quint64 m_centerFrequency;
		uint32_t m_tv_sec;
		uint32_t m_tv_usec;

		MsgReportSDRdaemonSourceStreamData(int sampleRate, quint64 centerFrequency, uint32_t tv_sec, uint32_t tv_usec) :
			Message(),
			m_sampleRate(sampleRate),
			m_centerFrequency(centerFrequency),
			m_tv_sec(tv_sec),
			m_tv_usec(tv_usec)
		{ }
	};

	class MsgReportSDRdaemonSourceStreamTiming : public Message {
		MESSAGE_CLASS_DECLARATION

	public:
		uint32_t get_tv_sec() const { return m_tv_sec; }
		uint32_t get_tv_usec() const { return m_tv_usec; }
		int getFramesDecodingStatus() const { return m_framesDecodingStatus; }
		bool allBlocksReceived() const { return m_allBlocksReceived; }
		float getBufferLengthInSecs() const { return m_bufferLenSec; }
        int32_t getBufferGauge() const { return m_bufferGauge; }
        int getMinNbBlocks() const { return m_minNbBlocks; }
        int getMinNbOriginalBlocks() const { return m_minNbOriginalBlocks; }
        int getMaxNbRecovery() const { return m_maxNbRecovery; }
        float getAvgNbBlocks() const { return m_avgNbBlocks; }
        float getAvgNbOriginalBlocks() const { return m_avgNbOriginalBlocks; }
        float getAvgNbRecovery() const { return m_avgNbRecovery; }
        int getNbOriginalBlocksPerFrame() const { return m_nbOriginalBlocksPerFrame; }
        int getNbFECBlocksPerFrame() const { return m_nbFECBlocksPerFrame; }

		static MsgReportSDRdaemonSourceStreamTiming* create(uint32_t tv_sec,
				uint32_t tv_usec,
				float bufferLenSec,
                int32_t bufferGauge,
                int framesDecodingStatus,
                bool allBlocksReceived,
                int minNbBlocks,
                int minNbOriginalBlocks,
                int maxNbRecovery,
                float avgNbBlocks,
                float avgNbOriginalBlocks,
                float avgNbRecovery,
                int nbOriginalBlocksPerFrame,
                int nbFECBlocksPerFrame)
		{
			return new MsgReportSDRdaemonSourceStreamTiming(tv_sec,
					tv_usec,
					bufferLenSec,
                    bufferGauge,
                    framesDecodingStatus,
                    allBlocksReceived,
                    minNbBlocks,
                    minNbOriginalBlocks,
                    maxNbRecovery,
                    avgNbBlocks,
                    avgNbOriginalBlocks,
                    avgNbRecovery,
                    nbOriginalBlocksPerFrame,
                    nbFECBlocksPerFrame);
		}

	protected:
		uint32_t m_tv_sec;
		uint32_t m_tv_usec;
		int      m_framesDecodingStatus;
		bool     m_allBlocksReceived;
		float    m_bufferLenSec;
        int32_t  m_bufferGauge;
        int      m_minNbBlocks;
        int      m_minNbOriginalBlocks;
        int      m_maxNbRecovery;
        float    m_avgNbBlocks;
        float    m_avgNbOriginalBlocks;
        float    m_avgNbRecovery;
        int      m_nbOriginalBlocksPerFrame;
        int      m_nbFECBlocksPerFrame;

		MsgReportSDRdaemonSourceStreamTiming(uint32_t tv_sec,
				uint32_t tv_usec,
				float bufferLenSec,
                int32_t bufferGauge,
                int framesDecodingStatus,
                bool allBlocksReceived,
                int minNbBlocks,
                int minNbOriginalBlocks,
                int maxNbRecovery,
                float avgNbBlocks,
                float avgNbOriginalBlocks,
                float avgNbRecovery,
                int nbOriginalBlocksPerFrame,
                int nbFECBlocksPerFrame) :
			Message(),
			m_tv_sec(tv_sec),
			m_tv_usec(tv_usec),
			m_framesDecodingStatus(framesDecodingStatus),
			m_allBlocksReceived(allBlocksReceived),
			m_bufferLenSec(bufferLenSec),
            m_bufferGauge(bufferGauge),
            m_minNbBlocks(minNbBlocks),
            m_minNbOriginalBlocks(minNbOriginalBlocks),
            m_maxNbRecovery(maxNbRecovery),
            m_avgNbBlocks(avgNbBlocks),
            m_avgNbOriginalBlocks(avgNbOriginalBlocks),
            m_avgNbRecovery(avgNbRecovery),
            m_nbOriginalBlocksPerFrame(nbOriginalBlocksPerFrame),
            m_nbFECBlocksPerFrame(nbFECBlocksPerFrame)
		{ }
	};

    class MsgFileRecord : public Message {
        MESSAGE_CLASS_DECLARATION

    public:
        bool getStartStop() const { return m_startStop; }

        static MsgFileRecord* create(bool startStop) {
            return new MsgFileRecord(startStop);
        }

    protected:
        bool m_startStop;

        MsgFileRecord(bool startStop) :
            Message(),
            m_startStop(startStop)
        { }
    };

	SDRdaemonSourceInput(DeviceSourceAPI *deviceAPI);
	virtual ~SDRdaemonSourceInput();
	virtual void destroy();

	virtual bool start();
	virtual void stop();

	virtual const QString& getDeviceDescription() const;
	virtual int getSampleRate() const;
	virtual quint64 getCenterFrequency() const;
	std::time_t getStartingTimeStamp() const;
	void getRemoteAddress(QString &s);

	virtual bool handleMessage(const Message& message);

	virtual void setMessageQueueToGUI(MessageQueue *queue);

private:
	DeviceSourceAPI *m_deviceAPI;
	QMutex m_mutex;
	SDRdaemonSourceSettings m_settings;
	QString m_address;
	quint16 m_port;
	SDRdaemonSourceUDPHandler* m_SDRdaemonUDPHandler;
	QString m_deviceDescription;
	std::time_t m_startingTimeStamp;
	const QTimer& m_masterTimer;
    bool m_autoFollowRate;
    bool m_autoCorrBuffer;
    FileRecord *m_fileSink; //!< File sink to record device I/Q output
};

#endif // INCLUDE_SDRDAEMONSOURCEINPUT_H
