#include "core.h"
#include "storages/desktopdevice.h"

#include <QDebug>

Core::Core(QObject *parent)
    : QObject(parent)
{
    m_connector = new HWConnector(this);
    m_deviceCreator = new DeviceBuilder(this);

    connect(m_connector, &HWConnector::connectionStatusChanged, this, &Core::onStatusChanged);
    connect(m_connector, &HWConnector::documentReceived, m_deviceCreator, &DeviceBuilder::onDocumentRecieved);

    connect(m_deviceCreator, &DeviceBuilder::desktopCreated, this, &Core::onDeviceCreated);
}

Core::~Core()
{
}

void Core::onMakeGetRequest(const QString &target)
{
    setState(SessionState::CONNECTING);
    m_connector->makeGetRequest(target);
}

void Core::onCloseConnection()
{
    setState(SessionState::DISCONNECTED);
    m_connector->closeConnection();
}

void Core::onDeviceCreated(DesktopDevice *device)
{
    // Core is a non-owning observer by design. Ownership stays outside Core.
    m_device = device;

    setState(SessionState::CONNECTED);
    emit deviceReady(device);
}

void Core::onStatusChanged(HWConnector::ConnectionStatus status)
{
}

QObject *Core::device() const
{
    return m_device;
}

bool Core::isValidTransition(SessionState from, SessionState to) const
{
    switch (from) {
    case SessionState::IDLE:
        return to == SessionState::CONNECTING;
    case SessionState::CONNECTING:
        return to == SessionState::CONNECTED
               || to == SessionState::ERROR
               || to == SessionState::DISCONNECTED;
    case SessionState::CONNECTED:
        return to == SessionState::DISCONNECTED
               || to == SessionState::ERROR;
    case SessionState::ERROR:
    case SessionState::DISCONNECTED:
        return to == SessionState::CONNECTING;
    }

    return false;
}

void Core::setState(SessionState newState, const QString &errorText)
{
    const SessionState oldState = m_state;

    if (oldState == newState) {
        return;
    }

    if (!isValidTransition(oldState, newState)) {
        qWarning().noquote() << QStringLiteral("Core: invalid transition %1 -> %2")
                                    .arg(SessionStateNs::toString(oldState),
                                         SessionStateNs::toString(newState));
        return;
    }

    m_state = newState;
    qInfo().noquote() << QStringLiteral("Core: state %1 -> %2")
                             .arg(SessionStateNs::toString(oldState),
                                  SessionStateNs::toString(newState));

    if (!errorText.isEmpty()) {
        qWarning().noquote() << QStringLiteral("Core error: %1").arg(errorText);
    }

    emit sessionStateChanged(newState);
}
