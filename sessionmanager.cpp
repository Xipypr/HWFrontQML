#include "sessionmanager.h"

#include <QDebug>

SessionManager::SessionManager(QObject *parent)
    : QObject(parent)
{
}

SessionManager::~SessionManager()
{
    const QList<SessionEntry> sessions = m_sessions.values();
    m_sessions.clear();
    m_sessionIdsByName.clear();

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
    session.displayName = target;

    if (m_sessions.contains(session.sessionId)) {
        qWarning().noquote() << QStringLiteral("Refusing to create duplicated session id: %1").arg(session.sessionId);
        return QString();
    }

    Core *core = new Core(this);

    connect(core, &Core::sessionStateChanged, this, [this, sessionId = session.sessionId](const QString &state) {
        emit sessionStateChanged(sessionId, state);
    });
    connect(core, &Core::deviceReady, this, [this, sessionId = session.sessionId](QObject *deviceRef) {
        emit deviceReady(sessionId, deviceRef);
    });

    connect(core, &QObject::destroyed, this, [this, sessionId = session.sessionId]() {
        const SessionEntry removedEntry = m_sessions.take(sessionId);
        if (!removedEntry.session.sessionId.isEmpty()) {
            m_sessionIdsByName.remove(removedEntry.session.displayName, sessionId);
            emit sessionRemoved(sessionId);
            emit sessionIdsChanged();
        }
    });

    SessionEntry entry;
    entry.session = session;
    entry.core = core;
    m_sessions.insert(session.sessionId, entry);
    m_sessionIdsByName.insert(session.displayName, session.sessionId);

    emit sessionCreated(session.sessionId);
    emit sessionIdsChanged();

    core->onMakeGetRequest(target);
    return session.sessionId;
}

void SessionManager::removeSession(const QString &sessionId)
{
    if (sessionId.isEmpty()) {
        qWarning() << "Skipping removeSession: empty sessionId";
        return;
    }

    const SessionEntry entry = m_sessions.take(sessionId);
    if (!entry.core) {
        qInfo().noquote() << QStringLiteral("Skipping removeSession: session '%1' was already removed").arg(sessionId);
        return;
    }

    m_sessionIdsByName.remove(entry.session.displayName, sessionId);
    disconnect(entry.core, nullptr, this, nullptr);
    emit sessionRemoved(sessionId);
    emit sessionIdsChanged();
    entry.core->deleteLater();
}

QObject *SessionManager::coreForSession(const QString &sessionId) const
{
    return m_sessions.value(sessionId).core;
}

QObject *SessionManager::coreForSessionName(const QString &sessionName) const
{
    const QString sessionId = sessionIdByName(sessionName);
    return coreForSession(sessionId);
}

QString SessionManager::sessionIdByName(const QString &sessionName) const
{
    const QList<QString> ids = m_sessionIdsByName.values(sessionName);
    if (ids.isEmpty()) {
        return QString();
    }

    return ids.first();
}

QStringList SessionManager::sessionIds() const
{
    return m_sessions.keys();
}
