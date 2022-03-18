#ifndef CORE_H
#define CORE_H

#include "hwconnector.h"
#include "devicebuilder.h"

#include <QObject>

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core();
    ~Core();

public slots:
    void onStartMonitoring();
    void onMakeGetRequest(const QString & target);

    void onDeviceCreated(DesktopDevice * device);

    Q_INVOKABLE QObject * getDevice();

signals:
    void deviceCreated(DesktopDevice * device);

    void testSignal();

private:
    HWConnector * m_connector;
    DeviceBuilder * m_deviceCreator;
    Device * tmpDevice;

};

#endif // CORE_H
