#include "core.h"
#include "storages/device.h"
#include "storages/desktopdevice.h"

#include <QDateTime>
#include <QDebug>
#include <QMetaEnum>
#include <QUuid>

Core::Core()
{
    qRegisterMetaType<Core::SessionState>("Core::SessionState");
    qRegisterMetaType<Core::Session>("Core::Session");

    m_connector = new HWConnector(this);
    m_deviceCreator = new DeviceBuilder(this);
    connect(m_connector, &HWConnector::documentRecieved, m_deviceCreator, &DeviceBuilder::onDocumentRecieved);
    connect(m_deviceCreator, &DeviceBuilder::desktopCreated, this, &Core::onDeviceCreated);

    m_session.sessionId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_session.target = QString();
    m_session.displayName = QStringLiteral("No device");
    m_session.state = Core::SessionState::idle;
    m_session.lastError = QString();
    m_session.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

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
    setSessionState(Core::SessionState::connecting);
    m_connector->makeGetRequest(target);
}

void Core::onDeviceCreated(DesktopDevice *device)
{
    // Core is a non-owning observer by design. Ownership stays outside Core.
    m_device = device;
    m_session.displayName = (m_device && !m_device->name.isEmpty()) ? m_device->name : m_session.target;
    setSessionState(Core::SessionState::connected);
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
    setSessionState(Core::SessionState::disconnected);
}

Core::SessionState Core::sessionState() const
{
    return m_session.state;
}

QVariantMap Core::sessionToMap() const
{
    const QMetaEnum enumMeta = QMetaEnum::fromType<Core::SessionState>();

    QVariantMap map;
    map.insert(QStringLiteral("sessionId"), m_session.sessionId);
    map.insert(QStringLiteral("target"), m_session.target);
    map.insert(QStringLiteral("displayName"), m_session.displayName);
    map.insert(QStringLiteral("state"), QString::fromLatin1(enumMeta.valueToKey(static_cast<int>(m_session.state))));
    map.insert(QStringLiteral("lastError"), m_session.lastError);
    map.insert(QStringLiteral("createdAt"), m_session.createdAt);
    return map;
}

void Core::setSessionState(Core::SessionState newState, const QString &errorText)
{
    if (m_session.state == newState && m_session.lastError == errorText)
        return;

    m_session.state = newState;
    m_session.lastError = errorText;

    emit sessionStateChanged(m_session.state);
    emit sessionStateChangedById(m_session.sessionId, m_session.state);
    emit sessionChanged();
}
