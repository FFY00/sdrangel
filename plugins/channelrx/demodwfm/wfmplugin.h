#ifndef INCLUDE_WFMPLUGIN_H
#define INCLUDE_WFMPLUGIN_H

#include <QObject>
#include "plugin/plugininterface.h"

class DeviceUISet;
class BasebandSampleSink;

class WFMPlugin : public QObject, PluginInterface {
	Q_OBJECT
	Q_INTERFACES(PluginInterface)
	Q_PLUGIN_METADATA(IID "de.maintech.sdrangelove.channel.wfm")

public:
	explicit WFMPlugin(QObject* parent = NULL);

	const PluginDescriptor& getPluginDescriptor() const;
	void initPlugin(PluginAPI* pluginAPI);

	PluginInstanceGUI* createRxChannelGUI(const QString& channelName, DeviceUISet *deviceUISet, BasebandSampleSink *rxChannel);
    BasebandSampleSink* createRxChannel(const QString& channelName, DeviceSourceAPI *deviceAPI);

private:
	static const PluginDescriptor m_pluginDescriptor;

	PluginAPI* m_pluginAPI;
};

#endif // INCLUDE_WFMPLUGIN_H
