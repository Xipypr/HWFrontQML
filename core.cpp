#include "core.h"
#include "storages/device.h"
#include "storages/desktopdevice.h"

#include <QDebug>
#include <QUuid>

Core::Core()
{
    qRegisterMetaType<SessionState>("SessionState");
    qRegisterMetaType<Session>("Session");

    m_connector = new HWConnector(this);
    m_deviceCreator = new DeviceBuilder(this);
    connect(m_connector, &HWConnector::documentRecieved, m_deviceCreator, &DeviceBuilder::onDocumentRecieved);
    connect(m_deviceCreator, &DeviceBuilder::desktopCreated, this, &Core::onDeviceCreated);

    m_session = Session::createDefault();
    m_session.sessionId = QUuid::createUuid().toString(QUuid::WithoutBraces);

    emit sessionAdded(m_session.sessionId);
}

Core::~Core()
{

}

void Core::onStartMonitoring()
{

}

void Core::onMakeGetRequest(const QString &target)
{
    m_session.target = target.trimmed();
    m_session.displayName = m_session.target;
    emit sessionChanged();
    setSessionState(SessionState::connecting);
    m_connector->makeGetRequest(target);
}

void Core::onDeviceCreated(DesktopDevice *device)
{
    // Core is a non-owning observer by design. Ownership stays outside Core.
    m_device = device;
    updateSessionDisplayName();
    setSessionState(SessionState::connected);
    emit deviceCreated();
    emit deviceReady(m_session.sessionId, m_device);
}

QObject *Core::device() const
{
    return m_device;
}

QVariantMap Core::session() const
{
    return sessionToMap();
}

void Core::disconnectSession()
{
    m_device = nullptr;
    m_session.displayName = QStringLiteral("No device");
    setSessionState(SessionState::disconnected);
}

SessionState Core::sessionState() const
{
    return m_session.state;
}

QVariantMap Core::sessionToMap() const
{
    QVariantMap map;
    map.insert(QStringLiteral("sessionId"), m_session.sessionId);
    map.insert(QStringLiteral("target"), m_session.target);
    map.insert(QStringLiteral("displayName"), m_session.displayName);
    map.insert(QStringLiteral("state"), SessionStateNs::toString(m_session.state));
    map.insert(QStringLiteral("lastError"), m_session.lastError);
    map.insert(QStringLiteral("createdAt"), m_session.createdAt);
    return map;
}

void Core::setSessionState(SessionState newState, const QString &errorText)
{
    if (m_session.state == newState && m_session.lastError == errorText)
        return;

    m_session.state = newState;
    m_session.lastError = errorText;

    emit sessionStateChanged(m_session.state);
    emit sessionStateChangedById(m_session.sessionId, m_session.state);
    emit sessionChanged();
}

void Core::updateSessionDisplayName()
{
    if (!m_device) {
        m_session.displayName = m_session.target;
        return;
    }

    if (m_device->name.isEmpty()) {
        m_session.displayName = m_session.target;
        return;
    }

    m_session.displayName = m_device->name;
}
