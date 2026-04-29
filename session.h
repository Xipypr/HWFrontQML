#ifndef SESSION_H
#define SESSION_H

#include "sessionstate.h"

#include <QMetaType>
#include <QObject>
#include <QUuid>

struct Session
{
    Q_GADGET

    Q_PROPERTY(QString sessionId MEMBER sessionId)
    Q_PROPERTY(QString target MEMBER target)
    Q_PROPERTY(QString displayName MEMBER displayName)
    Q_PROPERTY(SessionState state MEMBER state)
    Q_PROPERTY(bool hasDevice MEMBER hasDevice)

public:
    Session()
        : sessionId(QUuid::createUuid().toString(QUuid::WithoutBraces))
    {
    }

    QString sessionId;
    QString target;
    QString displayName;
    SessionState state = SessionState::idle;
    bool hasDevice = false;

    static Session createDefault()
    {
        Session session;
        session.displayName = QStringLiteral("No device");
        return session;
    }
};

Q_DECLARE_METATYPE(Session)

#endif // SESSION_H
