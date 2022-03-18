#include "core.h"

Core::Core()
//    : tmpDevice(new Device())
{
    m_connector = new HWConnector(this);
    m_deviceCreator = new DeviceBuilder(this);
    connect( m_connector, &HWConnector::documentRecieved, m_deviceCreator, &DeviceBuilder::onDocumentRecieved );
    connect( m_deviceCreator, &DeviceBuilder::desktopCreated, this, &Core::deviceCreated );
    connect( m_deviceCreator, &DeviceBuilder::desktopCreated, this, &Core::onDeviceCreated );
    //    connector->makeGetRequest();
}

Core::~Core()
{
    delete tmpDevice;
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
    tmpDevice = device;
}

QObject *Core::getDevice()
{
    return tmpDevice;
}
