#include "core.h"
#include "storages/device.h"
#include "storages/desktopdevice.h"

#include <QDebug>

Core::Core()
{
    m_connector = new HWConnector(this);
    m_deviceCreator = new DeviceBuilder(this);
    connect(m_connector, &HWConnector::documentRecieved, m_deviceCreator, &DeviceBuilder::onDocumentRecieved);
    connect(m_deviceCreator, &DeviceBuilder::desktopCreated, this, &Core::onDeviceCreated);
}

Core::~Core()
{
}

void Core::onStartMonitoring()
{
}

void Core::onMakeGetRequest(const QString &sessionId, const QString &target)
{
    m_sessionId = sessionId;

    emit sessionStateChanged(m_sessionId, QStringLiteral("connecting"));
    m_connector->makeGetRequest(target);
}

void Core::onDeviceCreated(DesktopDevice *device)
{
    // Core is a non-owning observer by design. Ownership stays outside Core.
    m_device = device;

    if (m_sessionId.isEmpty()) {
        qWarning() << "Desktop device was created without session id";
        return;
    }

    emit sessionStateChanged(m_sessionId, QStringLiteral("connected"));
    emit deviceReady(m_sessionId, device);
}

QObject *Core::device() const
{
    return m_device;
}
