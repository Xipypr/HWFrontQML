#include "sessionmanager.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QSet>

namespace {
constexpr auto kSessionsStateKey = "sessions/state";

}

SessionManager::SessionManager(QObject *parent)
    : QObject(parent)
{
}

SessionManager::~SessionManager()
{
    const QList<SessionEntry> sessions = m_sessions.values();
    m_sessions.clear();

    for (const SessionEntry &entry : sessions) {
        if (entry.core) {
            entry.core->deleteLater();
        }
    }
}

QString SessionManager::createSession(const QString &target)
{
    Session session;
    session.target = target;

    while (m_sessions.contains(session.sessionId)) {
        session = Session();
        session.target = target;
    }

    Core *core = new Core(this);

    connect(core, &Core::sessionStateChanged, this, [this, sessionId = session.sessionId](SessionState state) {
        SessionEntry *entry = findSessionEntry(sessionId);
        if (!entry) {
            return;
        }
        entry->session.state = state;
        m_sessionsModel.setSessionState(sessionId, state);
        emit sessionStateChanged(sessionId, state);
    });

    connect(core, &Core::deviceReady, this, [this, sessionId = session.sessionId](QObject *deviceRef) {
        SessionEntry *entry = findSessionEntry(sessionId);
        if (!entry) {
            return;
        }
        entry->session.hasDevice = true;
        entry->session.displayName = deviceRef ? deviceRef->property("name").toString() : entry->session.displayName;
        m_sessionsModel.upsertSession(entry->session);
        emit connectedSessionIdsChanged();
        emit deviceReady(sessionId, deviceRef);
    });

    connect(core, &QObject::destroyed, this, [this, sessionId = session.sessionId]() {
        if (m_sessions.remove(sessionId) > 0) {
            m_sessionsModel.removeSession(sessionId);
            emit sessionRemoved(sessionId);
            emit sessionIdsChanged();
            emit connectedSessionIdsChanged();
        }
    });

    SessionEntry entry;
    entry.session = session;
    entry.core = core;
    m_sessions.insert(session.sessionId, entry);
    m_sessionsModel.upsertSession(session);

    emit sessionCreated(session.sessionId);
    emit sessionIdsChanged();
    saveSessionsState();

    return session.sessionId;
}

void SessionManager::removeSession(const QString &sessionId)
{
    if (sessionId.isEmpty()) {
        return;
    }

    const SessionEntry entry = m_sessions.take(sessionId);
    if (!entry.core) {
        return;
    }

    m_sessionsModel.removeSession(sessionId);
    emit sessionRemoved(sessionId);
    emit sessionIdsChanged();
    emit connectedSessionIdsChanged();
    saveSessionsState();
    entry.core->deleteLater();
}

QObject *SessionManager::coreForSession(const QString &sessionId) const
{
    return m_sessions.value(sessionId, SessionEntry{}).core;
}

QStringList SessionManager::sessionIds() const
{
    return m_sessions.keys();
}

QStringList SessionManager::connectedSessionIds() const
{
    // Keep the same chronological order as SessionListModel (oldest -> newest).
    return m_sessionsModel.connectedSessionIds();
}


void SessionManager::setDeviceAlias(const QString &sessionId, const QString &alias)
{
    SessionEntry *entry = findSessionEntry(sessionId);
    if (!entry) {
        return;
    }

    entry->session.alias = alias;
    m_sessionsModel.setSessionAlias(sessionId, alias);
    emit sessionAliasChanged(sessionId, alias);
    saveSessionsState();
}

QString SessionManager::deviceAlias(const QString &sessionId) const
{
    if (sessionId.isEmpty()) {
        return {};
    }

    return m_sessionsModel.deviceAliasForSession(sessionId);
}

QAbstractListModel *SessionManager::sessionsModel()
{
    return &m_sessionsModel;
}

void SessionManager::saveSessionsState() const
{
    QJsonArray sessions;
    sessions.reserve(m_sessions.size());

    for (auto it = m_sessions.cbegin(); it != m_sessions.cend(); ++it) {
        const Session &session = it.value().session;
        QJsonObject object;
        object.insert(QStringLiteral("sessionId"), session.sessionId);
        object.insert(QStringLiteral("target"), session.target);
        object.insert(QStringLiteral("alias"), session.alias);
        object.insert(QStringLiteral("displayName"), session.displayName);
        object.insert(QStringLiteral("hasDevice"), session.hasDevice);
        sessions.append(object);
    }

    QSettings settings;
    settings.setValue(QString::fromLatin1(kSessionsStateKey), QString::fromUtf8(QJsonDocument(sessions).toJson(QJsonDocument::Compact)));
}

void SessionManager::restoreSessionsState()
{
    QSettings settings;
    const QString serialized = settings.value(QString::fromLatin1(kSessionsStateKey)).toString();
    if (serialized.isEmpty()) {
        return;
    }

    const QJsonDocument document = QJsonDocument::fromJson(serialized.toUtf8());
    if (!document.isArray()) {
        return;
    }

    QSet<QString> restoredTargets;

    const QJsonArray sessions = document.array();
    for (const QJsonValue &value : sessions) {
        if (!value.isObject()) {
            continue;
        }

        const QJsonObject object = value.toObject();
        const QString sessionId = object.value(QStringLiteral("sessionId")).toString().trimmed();
        const QString target = object.value(QStringLiteral("target")).toString().trimmed();
        if (sessionId.isEmpty() || target.isEmpty()) {
            continue;
        }
        if (m_sessions.contains(sessionId) || restoredTargets.contains(target)) {
            continue;
        }

        Session session;
        session.sessionId = sessionId;
        session.target = target;
        session.alias = object.value(QStringLiteral("alias")).toString();
        session.displayName = object.value(QStringLiteral("displayName")).toString();
        session.hasDevice = object.value(QStringLiteral("hasDevice")).toBool(false);

        SessionEntry entry;
        entry.session = session;
        entry.core = nullptr;
        m_sessions.insert(session.sessionId, entry);
        m_sessionsModel.upsertSession(session);
        restoredTargets.insert(target);
    }

    emit sessionIdsChanged();
    emit connectedSessionIdsChanged();
}

void SessionManager::clearSavedSessionsState()
{
    QSettings settings;
    settings.remove(QString::fromLatin1(kSessionsStateKey));
}

SessionManager::SessionEntry *SessionManager::findSessionEntry(const QString &sessionId)
{
    // We intentionally avoid m_sessions[sessionId] here: operator[] inserts a
    // default element for missing keys, which can resurrect a removed session
    // when delayed async signals arrive.
    auto it = m_sessions.find(sessionId);
    return it == m_sessions.end() ? nullptr : &it.value();
}
