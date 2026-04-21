#include "core.h"
#include "storages/desktopdevice.h"

#include <QDebug>

Core::Core()
{
    m_connector = new HWConnector(this);
    m_deviceCreator = new DeviceBuilder(this);
    connect(m_connector, &HWConnector::documentRecieved, m_deviceCreator, &DeviceBuilder::onDocumentRecieved);

    //TODO FIX
    connect(m_connector, SIGNAL(errorOccurred(QString)), this, SLOT(onConnectorError(QString)));
    connect(m_connector, SIGNAL(disconnected()), this, SLOT(onConnectorDisconnected()));

    connect(m_deviceCreator, &DeviceBuilder::desktopCreated, this, &Core::onDeviceCreated);
}

Core::~Core()
{
}

void Core::onStartMonitoring()
{
}

void Core::onMakeGetRequest(const QString &target)
{
    setState(SessionState::connecting);
    m_connector->makeGetRequest(target);
}

void Core::onDeviceCreated(DesktopDevice *device)
{
    // Core is a non-owning observer by design. Ownership stays outside Core.
    m_device = device;

    setState(SessionState::connected);
    emit deviceReady(device);
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

    emit sessionStateChanged(SessionStateNs::toString(newState));
}
