#include "sessionmanager.h"


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

    connect(core, &Core::sessionStateChanged, this, [this, sessionId = session.sessionId](const QString &state) {
        emit sessionStateChanged(sessionId, state);
    });
    connect(core, &Core::deviceReady, this, [this, sessionId = session.sessionId](QObject *deviceRef) {
        emit deviceReady(sessionId, deviceRef);
    });

    connect(core, &QObject::destroyed, this, [this, sessionId = session.sessionId]() {
        if (m_sessions.remove(sessionId) > 0) {
            emit sessionRemoved(sessionId);
            emit sessionIdsChanged();
        }
    });

    SessionEntry entry;
    entry.session = session;
    entry.core = core;
    m_sessions.insert(session.sessionId, entry);

    emit sessionCreated(session.sessionId);
    emit sessionIdsChanged();

    core->onMakeGetRequest(target);
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

    emit sessionRemoved(sessionId);
    emit sessionIdsChanged();
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
