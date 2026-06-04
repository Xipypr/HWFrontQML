#include "core.h"

#include <lhmsnapshotparser.h>

#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>

Core::Core(QObject *parent)
    : QObject(parent)
{
    m_connector = new HWConnector(this);

    connect(m_connector, &HWConnector::connectionStatusChanged, this, &Core::onStatusChanged);
    connect(m_connector, &HWConnector::documentReceived, this, &Core::onDocumentReceived);
}

Core::~Core()
{
}

void Core::onStartConnection(const QString &target)
{
    m_connector->makeGetRequest(target);
}

void Core::onCloseConnection()
{
    m_connector->closeConnection();
}

void Core::onDocumentReceived(const QJsonObject &document)
{
    const LhmSnapshotParser parser;
    const HardwareSnapshot snapshot = parser.parse(document);

    emit snapshotReady(snapshot, displayNameFromDocument(document));
}

void Core::onStatusChanged(HWConnector::ConnectionStatus status)
{
    setState(convertConnectorEnum(status));
}

bool Core::isValidTransition(SessionState from, SessionState to) const
{
    switch (from) {
    case SessionState::IDLE:
        return to == SessionState::CONNECTING;
    case SessionState::CONNECTING:
        return to == SessionState::CONNECTED
               || to == SessionState::RECONNECTING
               || to == SessionState::ERROR
               || to == SessionState::DISCONNECTED;
    case SessionState::CONNECTED:
        return to == SessionState::RECONNECTING
               || to == SessionState::DISCONNECTED
               || to == SessionState::ERROR;
    case SessionState::RECONNECTING:
        return to == SessionState::CONNECTED
               || to == SessionState::ERROR
               || to == SessionState::DISCONNECTED;
    case SessionState::ERROR:
    case SessionState::DISCONNECTED:
        return to == SessionState::CONNECTING;
    }

    return false;
}

void Core::setState(SessionState newState)
{
    const SessionState oldState = m_state;

    if (oldState == newState) {
        return;
    }

    if (!isValidTransition(oldState, newState)) {
        return;
    }

    m_state = newState;

    emit sessionStateChanged(newState);
}

QString Core::displayNameFromDocument(const QJsonObject &document)
{
    const QJsonArray children = document.value(QStringLiteral("Children")).toArray();
    for (const QJsonValue &child : children) {
        if (!child.isObject())
            continue;

        const QString text = child.toObject().value(QStringLiteral("Text")).toString().trimmed();
        if (!text.isEmpty())
            return text;
    }

    return document.value(QStringLiteral("Text")).toString().trimmed();
}
