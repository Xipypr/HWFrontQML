#include <QMetaEnum>
#include "sessionmanager.h"

SessionManager::SessionManager(QObject *parent)
    : QObject(parent)
    , m_sessionsModel()
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
    return createSessionInternal(target, true);
}

QString SessionManager::appendSession()
{
    return createSessionInternal(QString(), false);
}

QString SessionManager::createSessionInternal(const QString &target, bool startRequest)
{
    Session session;
    session.target = target;
    session.displayName = target;

    while (m_sessions.contains(session.sessionId)) {
        session = Session();
        session.target = target;
        session.displayName = target;
    }

    Core *core = new Core(this);

    connect(core, &Core::sessionStateChanged, this, [this, sessionId = session.sessionId](const QString &state) {
        auto it = m_sessions.find(sessionId);
        if (it == m_sessions.end()) {
            return;
        }
        SessionEntry &entry = it.value();
        const QMetaEnum enumMeta = QMetaEnum::fromType<SessionState>();
        entry.session.state = static_cast<SessionState>(enumMeta.keyToValue(state.toLatin1().constData()));
        m_sessionsModel.setSessionState(sessionId, state);
        emit sessionStateChanged(sessionId, state);
    });
    connect(core, &Core::deviceReady, this, [this, sessionId = session.sessionId](QObject *deviceRef) {
        auto it = m_sessions.find(sessionId);
        if (it == m_sessions.end()) {
            return;
        }
        SessionEntry &entry = it.value();
        entry.session.hasDevice = true;
        entry.session.displayName = deviceRef ? deviceRef->property("name").toString() : entry.session.displayName;
        m_sessionsModel.upsertSession(entry.session);
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

    if (startRequest) {
        core->onMakeGetRequest(target);
    }
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
    QStringList ids;
    ids.reserve(m_sessions.size());
    for (auto it = m_sessions.constBegin(); it != m_sessions.constEnd(); ++it) {
        if (it.value().session.hasDevice) {
            ids.push_back(it.key());
        }
    }
    return ids;
}

QAbstractListModel *SessionManager::sessionsModel()
{
    return &m_sessionsModel;
}
