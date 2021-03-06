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


#include "SWGDVSeralDevices.h"

#include "SWGHelpers.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>
#include <QDebug>

namespace Swagger {

SWGDVSeralDevices::SWGDVSeralDevices(QString* json) {
    init();
    this->fromJson(*json);
}

SWGDVSeralDevices::SWGDVSeralDevices() {
    init();
}

SWGDVSeralDevices::~SWGDVSeralDevices() {
    this->cleanup();
}

void
SWGDVSeralDevices::init() {
    nb_devices = 0;
    dv_serial_devices = new QList<QString*>();
}

void
SWGDVSeralDevices::cleanup() {
    

    if(dv_serial_devices != nullptr) {
        QList<QString*>* arr = dv_serial_devices;
        foreach(QString* o, *arr) {
            delete o;
        }
        delete dv_serial_devices;
    }
}

SWGDVSeralDevices*
SWGDVSeralDevices::fromJson(QString &json) {
    QByteArray array (json.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
    return this;
}

void
SWGDVSeralDevices::fromJsonObject(QJsonObject &pJson) {
    ::Swagger::setValue(&nb_devices, pJson["nbDevices"], "qint32", "");
    
    ::Swagger::setValue(&dv_serial_devices, pJson["dvSerialDevices"], "QList", "QString");
    
}

QString
SWGDVSeralDevices::asJson ()
{
    QJsonObject* obj = this->asJsonObject();
    
    QJsonDocument doc(*obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject*
SWGDVSeralDevices::asJsonObject() {
    QJsonObject* obj = new QJsonObject();
    
    obj->insert("nbDevices", QJsonValue(nb_devices));

    QJsonArray dv_serial_devicesJsonArray;
    toJsonArray((QList<void*>*)dv_serial_devices, &dv_serial_devicesJsonArray, "dv_serial_devices", "QString");
    obj->insert("dvSerialDevices", dv_serial_devicesJsonArray);

    return obj;
}

qint32
SWGDVSeralDevices::getNbDevices() {
    return nb_devices;
}
void
SWGDVSeralDevices::setNbDevices(qint32 nb_devices) {
    this->nb_devices = nb_devices;
}

QList<QString*>*
SWGDVSeralDevices::getDvSerialDevices() {
    return dv_serial_devices;
}
void
SWGDVSeralDevices::setDvSerialDevices(QList<QString*>* dv_serial_devices) {
    this->dv_serial_devices = dv_serial_devices;
}


}

