#ifndef SESSIONSTATE_H
#define SESSIONSTATE_H

#include <QMetaEnum>
#include <QString>

namespace SessionStateNs {
Q_NAMESPACE

enum SessionState {
    IDLE,
    CONNECTING,
    CONNECTED,
    ERROR,
    DISCONNECTED,
    RECONNECTING
};
Q_ENUM_NS(SessionState)

inline QString toString(SessionState state)
{
    const QMetaEnum enumMeta = QMetaEnum::fromType<SessionState>();
    return QString::fromLatin1(enumMeta.valueToKey(static_cast<int>(state)));
}

} // namespace SessionStateNs

using SessionState = SessionStateNs::SessionState;
Q_DECLARE_METATYPE(SessionState)

#endif // SESSIONSTATE_H
