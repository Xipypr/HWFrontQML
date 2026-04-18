#ifndef SESSION_H
#define SESSION_H

#include "sessionstate.h"

#include <QDateTime>
#include <QMetaType>
#include <QObject>

struct Session
{
    Q_GADGET

    Q_PROPERTY(QString sessionId MEMBER sessionId)
    Q_PROPERTY(QString target MEMBER target)
    Q_PROPERTY(QString displayName MEMBER displayName)
    Q_PROPERTY(SessionState state MEMBER state)
    Q_PROPERTY(QString lastError MEMBER lastError)
    Q_PROPERTY(QString createdAt MEMBER createdAt)

public:
    QString sessionId;
    QString target;
    QString displayName;
    SessionState state = SessionState::idle;
    QString lastError;
    QString createdAt;

    static Session createDefault()
    {
        Session session;
        session.displayName = QStringLiteral("No device");
        session.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
        return session;
    }
};

Q_DECLARE_METATYPE(Session)

#endif // SESSION_H
