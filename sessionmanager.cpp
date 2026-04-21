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
        for (auto it = m_sessions.begin(); it != m_sessions.end(); ++it) {
            if (it.key().sessionId == sessionId) {
                m_sessions.erase(it);
                emit sessionRemoved(sessionId);
                emit sessionIdsChanged();
                return;
            }
        }
    });

    m_sessions.insert(session, core);

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

    for (auto it = m_sessions.begin(); it != m_sessions.end(); ++it) {
        if (it.key().sessionId != sessionId) {
            continue;
        }

        Core *core = it.value();
        m_sessions.erase(it);

        if (!core) {
            qInfo().noquote() << QStringLiteral("Skipping removeSession: session '%1' was already removed").arg(sessionId);
            return;
        }

        disconnect(core, nullptr, this, nullptr);
        emit sessionRemoved(sessionId);
        emit sessionIdsChanged();
        core->deleteLater();
        return;
    }

    qInfo().noquote() << QStringLiteral("Skipping removeSession: session '%1' was already removed").arg(sessionId);
}

QObject *SessionManager::coreForSession(const QString &sessionId) const
{
    for (auto it = m_sessions.cbegin(); it != m_sessions.cend(); ++it) {
        if (it.key().sessionId == sessionId) {
            return it.value();
        }
    }

    return nullptr;
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
