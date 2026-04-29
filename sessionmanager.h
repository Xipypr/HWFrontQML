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

public:
    explicit SessionManager(QObject *parent = nullptr);
    ~SessionManager() override;

    Q_INVOKABLE QString createSession(const QString &target);
    Q_INVOKABLE QString appendSession();
    Q_INVOKABLE void removeSession(const QString &sessionId);
    Q_INVOKABLE QObject *coreForSession(const QString &sessionId) const;
    Q_INVOKABLE QStringList sessionIds() const;
    Q_INVOKABLE QStringList connectedSessionIds() const;
    QAbstractListModel *sessionsModel();

signals:
    void sessionIdsChanged();
    void connectedSessionIdsChanged();
    void sessionsModelChanged();
    void sessionCreated(const QString &sessionId);
    void sessionRemoved(const QString &sessionId);
    void sessionStateChanged(const QString &sessionId, const QString &state);
    void deviceReady(const QString &sessionId, QObject *deviceRef);

private:
    QString createSessionInternal(const QString &target, bool startRequest);

    struct SessionEntry {
        Session session;
        Core *core = nullptr;
    };

    SessionEntry *findSessionEntry(const QString &sessionId);

    QMap<QString, SessionEntry> m_sessions;
    SessionListModel m_sessionsModel;
};

#endif // SESSIONMANAGER_H
