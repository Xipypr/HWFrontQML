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

public:
    Session()
        : sessionId(QUuid::createUuid().toString(QUuid::WithoutBraces))
    {
    }

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

inline bool operator<(const Session &lhs, const Session &rhs)
{
    return lhs.sessionId < rhs.sessionId;
}

inline bool operator==(const Session &lhs, const Session &rhs)
{
    return lhs.sessionId == rhs.sessionId;
}

inline uint qHash(const Session &session, uint seed = 0)
{
    return qHash(session.sessionId, seed);
}

Q_DECLARE_METATYPE(Session)

#endif // SESSION_H
