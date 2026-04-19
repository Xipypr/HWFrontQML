#include "core.h"
#include "storages/device.h"
#include "storages/desktopdevice.h"

#include <QDebug>

Core::Core()
{
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
    // Core is a non-owning observer by design. Ownership stays outside Core.
    m_device = device;
    emit deviceCreated();
}

QObject *Core::device() const
{
    return m_device;
}
