#include <QMetaEnum>
#include "sessionmanager.h"

SessionManager::SessionManager(QObject *parent)
    : QObject(parent)
    , m_sessionsModel(this)
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

QString SessionManager::createSession(const QString &target, bool startRequest)
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
        SessionEntry &entry = m_sessions[sessionId];
        const QMetaEnum enumMeta = QMetaEnum::fromType<SessionState>();
        entry.session.state = static_cast<SessionState>(enumMeta.keyToValue(state.toLatin1().constData()));
        if (entry.session.state != SessionState::error) {
            entry.lastError.clear();
        }
        m_sessionsModel.setSessionState(sessionId, state, entry.lastError);
        emit sessionStateChanged(sessionId, state);
    });
    connect(core, &Core::deviceReady, this, [this, sessionId = session.sessionId](QObject *deviceRef) {
        SessionEntry &entry = m_sessions[sessionId];
        entry.hasDevice = true;
        entry.session.displayName = deviceRef ? deviceRef->property("name").toString() : entry.session.displayName;
        m_sessionsModel.setDeviceReady(sessionId, entry.session.displayName);
        emit deviceReady(sessionId, deviceRef);
    });

    connect(core, &QObject::destroyed, this, [this, sessionId = session.sessionId]() {
        if (m_sessions.remove(sessionId) > 0) {
            m_sessionsModel.removeSession(sessionId);
            emit sessionRemoved(sessionId);
            emit sessionIdsChanged();
        }
    });

    SessionEntry entry;
    entry.session = session;
    entry.core = core;
    m_sessions.insert(session.sessionId, entry);
    m_sessionsModel.upsertSession(session, false, QString());

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

QAbstractListModel *SessionManager::sessionsModel()
{
    return &m_sessionsModel;
}
