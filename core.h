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

    Q_INVOKABLE QObject *device() const;
    Q_INVOKABLE QString deviceAlias(const QString &deviceName) const;
    Q_INVOKABLE void setDeviceAlias(const QString &deviceName, const QString &alias);

public slots:
    void onStartMonitoring();
    void onMakeGetRequest(const QString & target);

    void onDeviceCreated(DesktopDevice * device);


signals:
    void deviceCreated();

    void testSignal();
    void deviceAliasChanged(const QString &deviceName, const QString &alias);

private:
    HWConnector * m_connector;
    DeviceBuilder * m_deviceCreator;
    DesktopDevice * m_device;
};

#endif // CORE_H
