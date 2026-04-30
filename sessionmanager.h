#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "core.h"
#include "session.h"
#include "sessionlistmodel.h"

#include <QMap>
#include <QObject>
#include <QStringList>

class SessionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList sessionIds READ sessionIds NOTIFY sessionIdsChanged)
    Q_PROPERTY(QStringList connectedSessionIds READ connectedSessionIds NOTIFY connectedSessionIdsChanged)
    Q_PROPERTY(QAbstractListModel *sessionsModel READ sessionsModel NOTIFY sessionsModelChanged)
    Q_PROPERTY(QAbstractListModel *connectedSessionsModel READ connectedSessionsModel NOTIFY connectedSessionsModelChanged)

public:
    explicit SessionManager(QObject *parent = nullptr);
    ~SessionManager() override;

    Q_INVOKABLE QString createSession(const QString &target);
    Q_INVOKABLE void removeSession(const QString &sessionId);
    Q_INVOKABLE QObject *coreForSession(const QString &sessionId) const;
    Q_INVOKABLE QStringList sessionIds() const;
    Q_INVOKABLE QStringList connectedSessionIds() const;
    Q_INVOKABLE void setDeviceAlias(const QString &sessionId, const QString &alias);
    Q_INVOKABLE QString deviceAlias(const QString &sessionId) const;
    Q_INVOKABLE int sessionState(const QString &sessionId) const;
    QAbstractListModel *sessionsModel();
    QAbstractListModel *connectedSessionsModel();

signals:
    void sessionIdsChanged();
    void connectedSessionIdsChanged();
    void sessionsModelChanged();
    void connectedSessionsModelChanged();
    void sessionCreated(const QString &sessionId);
    void sessionRemoved(const QString &sessionId);
    void sessionStateChanged(const QString &sessionId, SessionState state);
    void deviceReady(const QString &sessionId, QObject *deviceRef);
    void sessionAliasChanged(const QString &sessionId, const QString &alias);

private:
    struct SessionEntry {
        Session session;
        Core *core = nullptr;
    };

    SessionEntry *findSessionEntry(const QString &sessionId);

    QMap<QString, SessionEntry> m_sessions;
    SessionListModel m_sessionsModel;
    QAbstractListModel *m_connectedSessionsModel = nullptr;
};

#endif // SESSIONMANAGER_H
