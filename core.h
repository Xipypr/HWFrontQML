#ifndef CORE_H
#define CORE_H

#include "devicebuilder.h"
#include "hwconnector.h"
#include "sessionstate.h"

#include <QObject>
#include <QPointer>

class Core : public QObject
{
    Q_OBJECT
    Q_PROPERTY(SessionState state READ state NOTIFY sessionStateChanged)
public:
    explicit Core(QObject *parent = nullptr);
    ~Core();

    Q_INVOKABLE QObject *device() const;
    SessionState state() const { return m_state; }

public slots:
    void onStartConnection(const QString &target);
    void onCloseConnection();
    void onDeviceCreated(DesktopDevice *device);
    void onStatusChanged(HWConnector::ConnectionStatus status);

signals:
    void sessionStateChanged(SessionState state);
    void deviceReady(QObject *deviceRef);

    void testSignal();

private:
    static constexpr SessionState convertConnectorEnum(HWConnector::ConnectionStatus status)
    {
        switch (status)
        {
            case HWConnector::CONNECTED:
                return SessionState::CONNECTED;

            case HWConnector::CONNECTING:
                return SessionState::CONNECTING;

            case HWConnector::DISCONNECTED:
                return SessionState::DISCONNECTED;

            case HWConnector::RECONNECTING:
                return SessionState::RECONNECTING;

            case HWConnector::ERROR:
                return SessionState::ERROR;
        }
    }

    bool isValidTransition(SessionState from, SessionState to) const;
    void setState(SessionState newState, const QString &errorText = QString());

    HWConnector *m_connector;
    DeviceBuilder *m_deviceCreator;
    // Non-owning guarded pointer: becomes nullptr automatically if deleted by owner.
    QPointer<DesktopDevice> m_device;
    SessionState m_state = SessionState::IDLE;
};

#endif // CORE_H
