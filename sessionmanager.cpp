#include "sessionmanager.h"

#include <QDebug>

SessionManager::SessionManager(QObject *parent)
    : QObject(parent)
{
}

SessionManager::~SessionManager()
{
    const QList<Core *> cores = m_sessions.values();
    m_sessions.clear();
    m_sessionIdsByName.clear();

    for (Core *core : cores) {
        if (core) {
            core->deleteLater();
        }
    }
}

QString SessionManager::createSession(const QString &target)
{
    Session session;
    session.target = target;
    session.displayName = target;

    if (m_sessions.contains(session)) {
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
        Session lookup;
        lookup.sessionId = sessionId;

        auto it = m_sessions.find(lookup);
        if (it == m_sessions.end()) {
            return;
        }

        const QString displayName = it.key().displayName;
        m_sessions.erase(it);
        m_sessionIdsByName.remove(displayName, sessionId);
        emit sessionRemoved(sessionId);
        emit sessionIdsChanged();
    });

    m_sessions.insert(session, core);
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

    Session lookup;
    lookup.sessionId = sessionId;

    auto it = m_sessions.find(lookup);
    if (it == m_sessions.end()) {
        qInfo().noquote() << QStringLiteral("Skipping removeSession: session '%1' was already removed").arg(sessionId);
        return;
    }

    const QString displayName = it.key().displayName;
    Core *core = it.value();
    m_sessions.erase(it);
    m_sessionIdsByName.remove(displayName, sessionId);

    if (!core) {
        qInfo().noquote() << QStringLiteral("Skipping removeSession: session '%1' was already removed").arg(sessionId);
        return;
    }

    disconnect(core, nullptr, this, nullptr);
    emit sessionRemoved(sessionId);
    emit sessionIdsChanged();
    core->deleteLater();
}

QObject *SessionManager::coreForSession(const QString &sessionId) const
{
    Session lookup;
    lookup.sessionId = sessionId;

    return m_sessions.value(lookup, nullptr);
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
    QStringList ids;
    ids.reserve(m_sessions.size());

    for (auto it = m_sessions.cbegin(); it != m_sessions.cend(); ++it) {
        ids.append(it.key().sessionId);
    }

    return ids;
}
