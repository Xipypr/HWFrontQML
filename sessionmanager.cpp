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

    for (const SessionEntry &entry : sessions) {
        if (entry.core) {
            entry.core->deleteLater();
        }
    }
}

QString SessionManager::createSession(const QString &target, const QString &displayName)
{
    Session session;
    session.target = target;
    session.displayName = displayName;

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
        const bool wasRemoved = m_sessions.remove(sessionId) > 0;
        if (wasRemoved) {
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
        qWarning() << "Skipping removeSession: empty sessionId";
        return;
    }

    const SessionEntry entry = m_sessions.take(sessionId);
    if (!entry.core) {
        qInfo().noquote() << QStringLiteral("Skipping removeSession: session '%1' was already removed").arg(sessionId);
        return;
    }

    disconnect(entry.core, nullptr, this, nullptr);
    emit sessionRemoved(sessionId);
    emit sessionIdsChanged();

    entry.core->deleteLater();
}

QObject *SessionManager::coreForSession(const QString &sessionId) const
{
    if (!m_sessions.contains(sessionId)) {
        return nullptr;
    }

    return m_sessions.value(sessionId).core;
}

QStringList SessionManager::sessionIds() const
{
    return m_sessions.keys();
}
