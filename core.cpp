#include "core.h"
#include "storages/device.h"
#include "storages/desktopdevice.h"

#include <QDebug>

Core::Core()
    : m_device(new DesktopDevice(this))
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
    const bool replacingCurrent = (m_device && m_device != device);
    const bool coreOwnsCurrent = replacingCurrent && (m_device->parent() == this);

    if (coreOwnsCurrent) {
        // Delete only objects that Core owns via QObject parent-child relation.
        m_device->deleteLater();
    }

    if (device && !device->parent()) {
        // Take ownership when builder returns an orphan object.
        device->setParent(this);
    }

    m_device = device;
    emit deviceCreated();
}

QObject *Core::device() const
{
    return m_device;
}
