/**
 * SDRangel
 * This is the web API of SDRangel SDR software. SDRangel is an Open Source Qt5/OpenGL 3.0+ GUI and server Software Defined Radio and signal analyzer in software. It supports Airspy, BladeRF, HackRF, LimeSDR, PlutoSDR, RTL-SDR, SDRplay RSP1 and FunCube
 *
 * OpenAPI spec version: 4.0.0
 * Contact: f4exb06@gmail.com
 *
 * NOTE: This class is auto generated by the swagger code generator program.
 * https://github.com/swagger-api/swagger-codegen.git
 * Do not edit the class manually.
 */


#include "SWGDeviceSetList.h"

#include "SWGHelpers.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>
#include <QDebug>

namespace Swagger {

SWGDeviceSetList::SWGDeviceSetList(QString* json) {
    init();
    this->fromJson(*json);
}

SWGDeviceSetList::SWGDeviceSetList() {
    init();
}

SWGDeviceSetList::~SWGDeviceSetList() {
    this->cleanup();
}

void
SWGDeviceSetList::init() {
    devicesetcount = 0;
    device_sets = new QList<SWGDeviceSet*>();
}

void
SWGDeviceSetList::cleanup() {
    

    if(device_sets != nullptr) {
        QList<SWGDeviceSet*>* arr = device_sets;
        foreach(SWGDeviceSet* o, *arr) {
            delete o;
        }
        delete device_sets;
    }
}

SWGDeviceSetList*
SWGDeviceSetList::fromJson(QString &json) {
    QByteArray array (json.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
    return this;
}

void
SWGDeviceSetList::fromJsonObject(QJsonObject &pJson) {
    ::Swagger::setValue(&devicesetcount, pJson["devicesetcount"], "qint32", "");
    
    ::Swagger::setValue(&device_sets, pJson["deviceSets"], "QList", "SWGDeviceSet");
    
}

QString
SWGDeviceSetList::asJson ()
{
    QJsonObject* obj = this->asJsonObject();
    
    QJsonDocument doc(*obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject*
SWGDeviceSetList::asJsonObject() {
    QJsonObject* obj = new QJsonObject();
    
    obj->insert("devicesetcount", QJsonValue(devicesetcount));

    QJsonArray device_setsJsonArray;
    toJsonArray((QList<void*>*)device_sets, &device_setsJsonArray, "device_sets", "SWGDeviceSet");
    obj->insert("deviceSets", device_setsJsonArray);

    return obj;
}

qint32
SWGDeviceSetList::getDevicesetcount() {
    return devicesetcount;
}
void
SWGDeviceSetList::setDevicesetcount(qint32 devicesetcount) {
    this->devicesetcount = devicesetcount;
}

QList<SWGDeviceSet*>*
SWGDeviceSetList::getDeviceSets() {
    return device_sets;
}
void
SWGDeviceSetList::setDeviceSets(QList<SWGDeviceSet*>* device_sets) {
    this->device_sets = device_sets;
}


}

