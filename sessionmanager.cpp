#include "sessionmanager.h"

#include <QDebug>
#include <QUuid>

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

QString SessionManager::createSession(const QString &target, const QString &displayName)
{
    const QString sessionId = generateUniqueSessionId();

    if (m_sessions.contains(sessionId)) {
        qWarning().noquote() << QStringLiteral("Refusing to create duplicated session id: %1").arg(sessionId);
        return QString();
    }

    Core *core = new Core();
    core->setParent(this);
    core->configureSession(sessionId, target, displayName);

    connect(core, &Core::sessionStateChanged, this, &SessionManager::sessionStateChanged);
    connect(core, &Core::deviceReady, this, &SessionManager::deviceReady);

    connect(core, &QObject::destroyed, this, [this, sessionId]() {
        const bool wasRemoved = m_sessions.remove(sessionId) > 0;
        if (wasRemoved) {
            emit sessionRemoved(sessionId);
            emit sessionIdsChanged();
        }
    });

    m_sessions.insert(sessionId, core);
    emit sessionCreated(sessionId);
    emit sessionIdsChanged();

    core->onMakeGetRequest(target);
    return sessionId;
}

void SessionManager::removeSession(const QString &sessionId)
{
    if (sessionId.isEmpty()) {
        qWarning() << "Skipping removeSession: empty sessionId";
        return;
    }

    Core *core = m_sessions.take(sessionId);
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
    return m_sessions.value(sessionId, nullptr);
}

QStringList SessionManager::sessionIds() const
{
    return m_sessions.keys();
}

QString SessionManager::generateUniqueSessionId() const
{
    QString sessionId;
    do {
        sessionId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    } while (m_sessions.contains(sessionId));

    return sessionId;
}
