#ifndef SESSION_H
#define SESSION_H

#include "sessionstate.h"

#include <QMetaType>
#include <QObject>

struct Session
{
    Q_GADGET

    Q_PROPERTY(QString sessionId MEMBER sessionId)
    Q_PROPERTY(QString target MEMBER target)
    Q_PROPERTY(QString displayName MEMBER displayName)
    Q_PROPERTY(SessionState state MEMBER state)

public:
    QString sessionId;
    QString target;
    QString displayName;
    SessionState state = SessionState::idle;

    static Session createDefault()
    {
        Session session;
        session.displayName = QStringLiteral("No device");
        return session;
    }
};

Q_DECLARE_METATYPE(Session)

#endif // SESSION_H
