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
    m_pendingSessionId = sessionId;
    emit sessionAdded(sessionId);
    emit sessionStateChanged(sessionId, QStringLiteral("connecting"));
    m_connector->makeGetRequest(target);
}

void Core::onRemoveSession(const QString &sessionId)
{
    if (m_pendingSessionId == sessionId) {
        m_pendingSessionId.clear();
    }

    if (m_activeSessionId == sessionId) {
        m_activeSessionId.clear();
        m_device.clear();
    }

    emit sessionStateChanged(sessionId, QStringLiteral("disconnected"));
    emit sessionRemoved(sessionId);
}

void Core::onDeviceCreated(DesktopDevice *device)
{
    // Core is a non-owning observer by design. Ownership stays outside Core.
    m_device = device;

    const QString sessionId = m_pendingSessionId;
    if (sessionId.isEmpty()) {
        qWarning() << "Desktop device was created without pending session id";
        return;
    }

    m_activeSessionId = sessionId;
    emit sessionStateChanged(sessionId, QStringLiteral("connected"));
    emit deviceReady(sessionId, device);

    m_pendingSessionId.clear();
}

QObject *Core::device() const
{
    return m_device;
}
