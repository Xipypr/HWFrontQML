#ifndef CORE_H
#define CORE_H

#include "hwconnector.h"
#include "devicebuilder.h"

#include <QObject>
#include <QPointer>
#include <QVariantMap>

class Core : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap session READ session NOTIFY sessionChanged)
    Q_PROPERTY(SessionState sessionState READ sessionState NOTIFY sessionStateChanged)

public:
    enum class SessionState {
        idle,
        connecting,
        connected,
        error,
        disconnected
    };
    Q_ENUM(SessionState)

    struct Session
    {
        Q_GADGET

        Q_PROPERTY(QString sessionId MEMBER sessionId)
        Q_PROPERTY(QString target MEMBER target)
        Q_PROPERTY(QString displayName MEMBER displayName)
        Q_PROPERTY(Core::SessionState state MEMBER state)
        Q_PROPERTY(QString lastError MEMBER lastError)
        Q_PROPERTY(QString createdAt MEMBER createdAt)

    public:
        QString sessionId;
        QString target;
        QString displayName;
        SessionState state = SessionState::idle;
        QString lastError;
        QString createdAt;
    };

    explicit Core();
    ~Core();

    Q_INVOKABLE QObject *device() const;
    Q_INVOKABLE QVariantMap session() const;
    Q_INVOKABLE void disconnectSession();

    SessionState sessionState() const;

public slots:
    void onStartMonitoring();
    void onMakeGetRequest(const QString & target);

    void onDeviceCreated(DesktopDevice * device);


signals:
    void deviceCreated();
    void sessionChanged();
    void sessionAdded(const QString &sessionId);
    void sessionStateChanged(SessionState state);
    void sessionStateChangedById(const QString &sessionId, SessionState state);
    void deviceReady(const QString &sessionId, QObject *deviceRef);
    void sessionRemoved(const QString &sessionId);

    void testSignal();

private:
    QVariantMap sessionToMap() const;
    void setSessionState(SessionState newState, const QString &errorText = QString());

private:
    HWConnector * m_connector;
    DeviceBuilder * m_deviceCreator;
    // Non-owning guarded pointer: becomes nullptr automatically if deleted by owner.
    QPointer<DesktopDevice> m_device;
    Session m_session;
};

Q_DECLARE_METATYPE(Core::SessionState)
Q_DECLARE_METATYPE(Core::Session)

#endif // CORE_H
