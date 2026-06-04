#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "core.h"
#include "session.h"
#include "../models/sessionlistmodel.h"
#include "../models/dashboardmetricsmodel.h"
#include "metricsservice.h"

#include <QMap>
#include <QObject>
#include <QStringList>

class SessionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList sessionIds READ sessionIds NOTIFY sessionIdsChanged)
    Q_PROPERTY(QStringList connectedSessionIds READ connectedSessionIds NOTIFY connectedSessionIdsChanged)
    Q_PROPERTY(QAbstractListModel *sessionsModel READ sessionsModel NOTIFY sessionsModelChanged)
    Q_PROPERTY(QAbstractItemModel *connectedSessionsModel READ connectedSessionsModel NOTIFY connectedSessionsModelChanged)
    Q_PROPERTY(bool persistSessionState READ persistSessionState WRITE setPersistSessionState NOTIFY persistSessionStateChanged)
    Q_PROPERTY(bool keepScreenAwake READ keepScreenAwake WRITE setKeepScreenAwake NOTIFY keepScreenAwakeChanged)

public:
    explicit SessionManager(QObject *parent = nullptr);
    ~SessionManager() override;

    Q_INVOKABLE QString createSession(const QString &target);
    Q_INVOKABLE void removeSession(const QString &sessionId);
    Q_INVOKABLE QObject *coreForSession(const QString &sessionId) const;
    Q_INVOKABLE QStringList sessionIds() const;
    Q_INVOKABLE QStringList connectedSessionIds() const;
    Q_INVOKABLE void setDeviceAlias(const QString &sessionId, const QString &alias);
    Q_INVOKABLE void setSessionTarget(const QString &sessionId, const QString &target);
    Q_INVOKABLE void saveSessionsState();
    Q_INVOKABLE void restoreSessionsState();
    Q_INVOKABLE void clearSavedSessionsState();
    Q_INVOKABLE bool persistSessionState() const;
    Q_INVOKABLE void setPersistSessionState(bool enabled);
    Q_INVOKABLE bool keepScreenAwake() const;
    Q_INVOKABLE void setKeepScreenAwake(bool enabled);
    Q_INVOKABLE QString deviceAlias(const QString &sessionId) const;
    Q_INVOKABLE DashboardMetricsModel *dashboardModelForSession(const QString &sessionId) const;
    QAbstractListModel *sessionsModel();
    Q_INVOKABLE int indexOfConnectedSession(const QString &sessionId) const;
    QAbstractItemModel *connectedSessionsModel();

signals:
    void sessionIdsChanged();
    void connectedSessionIdsChanged();
    void sessionsModelChanged();
    void connectedSessionsModelChanged();
    void sessionCreated(const QString &sessionId);
    void sessionRemoved(const QString &sessionId);
    void sessionStateChanged(const QString &sessionId, SessionState state);
    void deviceReady(const QString &sessionId, const QString &displayName);
    void sessionAliasChanged(const QString &sessionId, const QString &alias);
    void persistSessionStateChanged();
    void keepScreenAwakeChanged();

private:
    struct SessionEntry {
        Session session;
        Core *core = nullptr;
        DashboardMetricsModel *dashboardModel = nullptr;
        MetricsService *metricsService = nullptr;
    };

    SessionEntry *findSessionEntry(const QString &sessionId);
    SessionEntry createSessionEntry(const Session &session);
    bool hasSessionWithTarget(const QString &target) const;
    void applyKeepScreenAwake(bool enabled) const;

    QMap<QString, SessionEntry> m_sessions;
    SessionListModel m_sessionsModel;
    QAbstractItemModel *m_connectedSessionsModel = nullptr;
    bool m_keepScreenAwake = false;
};

#endif // SESSIONMANAGER_H
