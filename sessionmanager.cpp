#include "devicebuilder.h"
#include "sessionmanager.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QSortFilterProxyModel>

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


namespace {
constexpr auto SavedSessionsKey = "sessions/state";
constexpr auto PersistSessionStateKey = "app/persistSessionState";
constexpr bool DefaultPersistSessionState = true;
}

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

    SessionEntry entry = createSessionEntry(session);
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
    if (entry.dashboardModel) {
        entry.dashboardModel->deleteLater();
    }
    if (entry.metricsService) {
        entry.metricsService->deleteLater();
    }
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


void SessionManager::setSessionTarget(const QString &sessionId, const QString &target)
{
    SessionEntry *entry = findSessionEntry(sessionId);
    if (!entry || entry->session.target == target) {
        return;
    }

    entry->session.target = target;
    m_sessionsModel.upsertSession(entry->session);
    saveSessionsState();
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

void SessionManager::saveSessionsState()
{
    if (!persistSessionState()) {
        return;
    }

    QJsonArray sessionsArray;
    for (const SessionEntry &entry : m_sessions) {
        QJsonObject sessionObject;
        sessionObject[QStringLiteral("sessionId")] = entry.session.sessionId;
        sessionObject[QStringLiteral("target")] = entry.session.target;
        sessionObject[QStringLiteral("alias")] = entry.session.alias;
        sessionObject[QStringLiteral("displayName")] = entry.session.displayName;
        sessionObject[QStringLiteral("hasDevice")] = entry.session.hasDevice;
        if (entry.dashboardModel) {
            sessionObject[QStringLiteral("widgets")] = entry.dashboardModel->toJson();
        }
        sessionsArray.append(sessionObject);
    }

    QSettings settings;
    settings.setValue(QString::fromLatin1(SavedSessionsKey), QString::fromUtf8(QJsonDocument(sessionsArray).toJson(QJsonDocument::Compact)));
}

void SessionManager::restoreSessionsState()
{
    if (!persistSessionState()) {
        return;
    }

    const QString savedState = QSettings().value(QString::fromLatin1(SavedSessionsKey)).toString();
    if (savedState.isEmpty()) {
        return;
    }

    const QJsonDocument document = QJsonDocument::fromJson(savedState.toUtf8());
    if (!document.isArray()) {
        return;
    }

    bool restoredAnySession = false;
    bool restoredConnectedSession = false;

    for (const QJsonValue &value : document.array()) {
        if (!value.isObject()) {
            continue;
        }

        const QJsonObject sessionObject = value.toObject();
        const QString sessionId = sessionObject.value(QStringLiteral("sessionId")).toString().trimmed();
        const QString target = sessionObject.value(QStringLiteral("target")).toString().trimmed();

        if (sessionId.isEmpty() || m_sessions.contains(sessionId) || hasSessionWithTarget(target)) {
            continue;
        }

        Session session;
        session.sessionId = sessionId;
        session.target = target;
        session.alias = sessionObject.value(QStringLiteral("alias")).toString();
        session.displayName = sessionObject.value(QStringLiteral("displayName")).toString();
        session.hasDevice = sessionObject.value(QStringLiteral("hasDevice")).toBool(false);

        SessionEntry entry = createSessionEntry(session);
        const QJsonValue widgetsValue = sessionObject.value(QStringLiteral("widgets"));
        if (widgetsValue.isArray() && entry.dashboardModel) {
            entry.dashboardModel->restoreFromJson(widgetsValue.toArray());
        }
        m_sessions.insert(session.sessionId, entry);
        m_sessionsModel.upsertSession(session);
        restoredAnySession = true;
        restoredConnectedSession = restoredConnectedSession || session.hasDevice;
    }

    if (restoredAnySession) {
        emit sessionIdsChanged();
        emit sessionsModelChanged();
        if (restoredConnectedSession) {
            emit connectedSessionIdsChanged();
        }
    }
}

void SessionManager::clearSavedSessionsState()
{
    QSettings settings;
    settings.remove(QString::fromLatin1(SavedSessionsKey));
}

bool SessionManager::persistSessionState() const
{
    return QSettings().value(QString::fromLatin1(PersistSessionStateKey), DefaultPersistSessionState).toBool();
}

void SessionManager::setPersistSessionState(bool enabled)
{
    const bool wasEnabled = persistSessionState();

    QSettings settings;
    settings.setValue(QString::fromLatin1(PersistSessionStateKey), enabled);

    if (!enabled) {
        settings.remove(QString::fromLatin1(SavedSessionsKey));
    } else if (!wasEnabled) {
        saveSessionsState();
    }

    if (wasEnabled != enabled) {
        emit persistSessionStateChanged();
    }
}

SessionManager::SessionEntry SessionManager::createSessionEntry(const Session &session)
{
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
        saveSessionsState();
    });

    connect(core, &QObject::destroyed, this, [this, sessionId = session.sessionId]() {
        auto it = m_sessions.find(sessionId);
        if (it != m_sessions.end()) {
            if (it->dashboardModel) {
                it->dashboardModel->deleteLater();
                it->dashboardModel = nullptr;
            }
            if (it->metricsService) {
                it->metricsService->deleteLater();
                it->metricsService = nullptr;
            }
            m_sessions.erase(it);
            m_sessionsModel.removeSession(sessionId);
            emit sessionRemoved(sessionId);
            emit sessionIdsChanged();
            emit connectedSessionIdsChanged();
            saveSessionsState();
        }
    });

    SessionEntry entry;
    entry.session = session;
    entry.core = core;
    entry.dashboardModel = new DashboardMetricsModel(this);
    entry.metricsService = new MetricsService(this);
    connect(entry.dashboardModel, &DashboardMetricsModel::widgetsStateChanged, this, [this, sessionId = session.sessionId]() {
        if (findSessionEntry(sessionId)) {
            saveSessionsState();
        }
    });
    connect(core, &Core::deviceReady, entry.metricsService, &MetricsService::processDeviceSnapshot);
    connect(entry.metricsService, &MetricsService::availableMetricsChanged, entry.dashboardModel, &DashboardMetricsModel::onAvailableMetricsChanged);
    connect(entry.metricsService, &MetricsService::metricUpdated, entry.dashboardModel, &DashboardMetricsModel::onMetricUpdated);
    return entry;
}

bool SessionManager::hasSessionWithTarget(const QString &target) const
{
    if (target.isEmpty()) {
        return false;
    }

    for (const SessionEntry &entry : m_sessions) {
        if (entry.session.target == target) {
            return true;
        }
    }

    return false;
}

SessionManager::SessionEntry *SessionManager::findSessionEntry(const QString &sessionId)
{
    // We intentionally avoid m_sessions[sessionId] here: operator[] inserts a
    // default element for missing keys, which can resurrect a removed session
    // when delayed async signals arrive.
    auto it = m_sessions.find(sessionId);
    return it == m_sessions.end() ? nullptr : &it.value();
}
