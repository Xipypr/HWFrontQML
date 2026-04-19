#ifndef CORE_H
#define CORE_H

#include "devicebuilder.h"
#include "hwconnector.h"
#include "session.h"

#include <QObject>
#include <QPointer>
#include <QVariantMap>

class Core : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap session READ session NOTIFY sessionChanged)
    Q_PROPERTY(SessionState sessionState READ sessionState NOTIFY sessionStateChanged)

public:
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
    void setSessionState(SessionState newState);
    void updateSessionDisplayName();

private:
    HWConnector * m_connector;
    DeviceBuilder * m_deviceCreator;
    // Non-owning guarded pointer: becomes nullptr automatically if deleted by owner.
    QPointer<DesktopDevice> m_device;
    Session m_session;
};

#endif // CORE_H
