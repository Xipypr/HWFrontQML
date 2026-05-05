#include "devicebuilder.h"
#include <QSortFilterProxyModel>
#include "sessionmanager.h"

class ConnectedSessionsProxyModel : public QSortFilterProxyModel
{
public:
    using QSortFilterProxyModel::QSortFilterProxyModel;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override
    {
        const QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);
        return sourceModel()->data(idx, SessionListModel::HasDeviceRole).toBool();
    }
};

SessionManager::SessionManager(QObject *parent)
    : QObject(parent)
{
    auto *proxy = new ConnectedSessionsProxyModel(this);
    proxy->setSourceModel(&m_sessionsModel);
    proxy->setDynamicSortFilter(true);
    m_connectedSessionsModel = proxy;
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

    connect(core, &Core::deviceReady, this, [this, sessionId = session.sessionId](DesktopDevice *deviceRef) {
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
        auto it = m_sessions.find(sessionId);
        if (it != m_sessions.end()) {
            if (it->dashboardModel) {
                it->dashboardModel->deleteLater();
                it->dashboardModel = nullptr;
            }
            m_sessions.erase(it);
            m_sessionsModel.removeSession(sessionId);
            emit sessionRemoved(sessionId);
            emit sessionIdsChanged();
            emit connectedSessionIdsChanged();
        }
    });

    SessionEntry entry;
    entry.session = session;
    entry.core = core;
    entry.dashboardModel = new DashboardMetricsModel(this);
    entry.dashboardModel->setSessionId(session.sessionId);
    connect(core, &Core::deviceReady, entry.dashboardModel, &DashboardMetricsModel::onDeviceSnapshotReady);
    m_sessions.insert(session.sessionId, entry);
    m_sessionsModel.upsertSession(session);

    emit sessionCreated(session.sessionId);
    emit sessionIdsChanged();

    return session.sessionId;
}

void SessionManager::removeSession(const QString &sessionId)
{
    if (sessionId.isEmpty()) {
        return;
    }

    const SessionEntry entry = m_sessions.take(sessionId);
    if (entry.dashboardModel) {
        entry.dashboardModel->deleteLater();
    }
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
}

QString SessionManager::deviceAlias(const QString &sessionId) const
{
    if (sessionId.isEmpty()) {
        return {};
    }

    return m_sessionsModel.deviceAliasForSession(sessionId);
}


DashboardMetricsModel *SessionManager::dashboardModelForSession(const QString &sessionId) const
{
    if (sessionId.isEmpty())
        return nullptr;

    return m_sessions.value(sessionId, SessionEntry{}).dashboardModel;
}

QAbstractListModel *SessionManager::sessionsModel()
{
    return &m_sessionsModel;
}

int SessionManager::indexOfConnectedSession(const QString &sessionId) const
{
    if (sessionId.isEmpty() || !m_connectedSessionsModel) {
        return -1;
    }

    const int count = m_connectedSessionsModel->rowCount();
    for (int i = 0; i < count; ++i) {
        const QModelIndex index = m_connectedSessionsModel->index(i, 0);
        if (m_connectedSessionsModel->data(index, SessionListModel::SessionIdRole).toString() == sessionId) {
            return i;
        }
    }

    return -1;
}

QAbstractItemModel *SessionManager::connectedSessionsModel()
{
    return m_connectedSessionsModel;
}

SessionManager::SessionEntry *SessionManager::findSessionEntry(const QString &sessionId)
{
    // We intentionally avoid m_sessions[sessionId] here: operator[] inserts a
    // default element for missing keys, which can resurrect a removed session
    // when delayed async signals arrive.
    auto it = m_sessions.find(sessionId);
    return it == m_sessions.end() ? nullptr : &it.value();
}
