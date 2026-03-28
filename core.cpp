#include "core.h"
#include "storages/device.h"
#include "storages/desktopdevice.h"

#include <QDebug>

Core::Core()
    : m_device(new DesktopDevice())
{
//    qDebug() << m_device->name();
    m_connector = new HWConnector(this);
    m_deviceCreator = new DeviceBuilder(this);
    connect( m_connector, &HWConnector::documentRecieved, m_deviceCreator, &DeviceBuilder::onDocumentRecieved );
    connect( m_deviceCreator, &DeviceBuilder::desktopCreated, this, &Core::onDeviceCreated );
}

Core::~Core()
{

}

void Core::onStartMonitoring()
{

}

void Core::onMakeGetRequest(const QString &target)
{
    m_connector->makeGetRequest(target);
}

void Core::onDeviceCreated(DesktopDevice *device)
{
    m_device = device;
    emit deviceCreated();
}

QObject *Core::device() const
{
    return m_device;
}


QString Core::deviceAlias(const QString &deviceName) const
{
    if (deviceName.isEmpty())
        return QString();

    return m_aliases.value(deviceName);
}

void Core::setDeviceAlias(const QString &deviceName, const QString &alias)
{
    if (deviceName.isEmpty())
        return;

    if (alias.isEmpty())
        m_aliases.remove(deviceName);
    else
        m_aliases[deviceName] = alias;

    emit deviceAliasChanged(deviceName, alias);
}
