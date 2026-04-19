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
    // Non-owning guarded pointer: becomes nullptr automatically if deleted by owner.
    QPointer<DesktopDevice> m_device;
};

#endif // CORE_H
