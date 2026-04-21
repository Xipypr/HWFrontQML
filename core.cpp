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
    connect(m_connector, SIGNAL(errorOccurred(QString)), this, SLOT(onConnectorError(QString)));
    connect(m_connector, SIGNAL(disconnected()), this, SLOT(onConnectorDisconnected()));
}

Core::~Core()
{
}

void Core::onStartMonitoring()
{
}

void Core::onMakeGetRequest(const QString &target)
{
    m_session.target = target;

    setState(SessionState::connecting);
    m_connector->makeGetRequest(target);
}

void Core::onDeviceCreated(DesktopDevice *device)
{
    // Core is a non-owning observer by design. Ownership stays outside Core.
    m_device = device;

    if (m_session.sessionId.isEmpty()) {
        qWarning() << "Desktop device was created without session id";
        return;
    }

    setState(SessionState::connected);
    emit deviceReady(m_session.sessionId, device);
}

void Core::onConnectorError(const QString &errorText)
{
    setState(SessionState::error, errorText);
}

void Core::onConnectorDisconnected()
{
    setState(SessionState::disconnected);
}

QObject *Core::device() const
{
    return m_device;
}

QString Core::sessionId() const
{
    return m_session.sessionId;
}

bool Core::isValidTransition(SessionState from, SessionState to) const
{
    switch (from) {
    case SessionState::idle:
        return to == SessionState::connecting;
    case SessionState::connecting:
        return to == SessionState::connected
               || to == SessionState::error
               || to == SessionState::disconnected;
    case SessionState::connected:
        return to == SessionState::disconnected
               || to == SessionState::error;
    case SessionState::error:
    case SessionState::disconnected:
        return to == SessionState::connecting;
    }

    return false;
}

void Core::setState(SessionState newState, const QString &errorText)
{
    const SessionState oldState = m_session.state;

    if (oldState == newState) {
        return;
    }

    if (!isValidTransition(oldState, newState)) {
        qWarning().noquote() << QStringLiteral("Session %1: invalid transition %2 -> %3")
                                    .arg(m_session.sessionId,
                                         SessionStateNs::toString(oldState),
                                         SessionStateNs::toString(newState));
        return;
    }

    m_session.state = newState;
    qInfo().noquote() << QStringLiteral("Session %1: state %2 -> %3")
                             .arg(m_session.sessionId,
                                  SessionStateNs::toString(oldState),
                                  SessionStateNs::toString(newState));

    if (!errorText.isEmpty()) {
        qWarning().noquote() << QStringLiteral("Session %1: %2")
                                    .arg(m_session.sessionId, errorText);
    }

    // State changes are scoped by sessionId and stay local to this Core instance.
    emit sessionStateChanged(m_session.sessionId, SessionStateNs::toString(newState));
}
