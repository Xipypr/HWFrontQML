#ifndef CORE_H
#define CORE_H

#include "hwconnector.h"
#include "devicebuilder.h"

#include <QObject>
#include <QPointer>

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core();
    ~Core();

    Q_INVOKABLE QObject *device() const;

public slots:
    void onStartMonitoring();
    void onMakeGetRequest(const QString & target);

    void onDeviceCreated(DesktopDevice * device);


signals:
    void deviceCreated();

    void testSignal();

private:
    HWConnector * m_connector;
    DeviceBuilder * m_deviceCreator;
    // Guarded pointer: becomes nullptr automatically if DesktopDevice is deleted elsewhere.
    QPointer<DesktopDevice> m_device;
};

#endif // CORE_H
