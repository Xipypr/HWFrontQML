#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "core.h"
#include "session.h"

#include <QHash>
#include <QObject>
#include <QStringList>

class SessionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList sessionIds READ sessionIds NOTIFY sessionIdsChanged)

public:
    explicit SessionManager(QObject *parent = nullptr);
    ~SessionManager() override;

    Q_INVOKABLE QString createSession(const QString &target);
    Q_INVOKABLE void removeSession(const QString &sessionId);
    Q_INVOKABLE QObject *coreForSession(const QString &sessionId) const;
    Q_INVOKABLE QObject *coreForSessionName(const QString &sessionName) const;
    Q_INVOKABLE QString sessionIdByName(const QString &sessionName) const;
    Q_INVOKABLE QStringList sessionIds() const;

signals:
    void sessionIdsChanged();
    void sessionCreated(const QString &sessionId);
    void sessionRemoved(const QString &sessionId);
    void sessionStateChanged(const QString &sessionId, const QString &state);
    void deviceReady(const QString &sessionId, QObject *deviceRef);

private:
    struct SessionEntry {
        Session session;
        Core *core = nullptr;
    };

    QHash<QString, SessionEntry> m_sessions;
    QMultiHash<QString, QString> m_sessionIdsByName;
};

#endif // SESSIONMANAGER_H
