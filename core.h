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
public:
    explicit Core(QObject *parent = nullptr);
    ~Core();

    Q_INVOKABLE QObject *device() const;

public slots:
    void onMakeGetRequest(const QString &target);
    void onDeviceCreated(DesktopDevice *device);
    void onConnectorError(const QString &errorText);
    void onConnectorDisconnected();

signals:
    void sessionStateChanged(SessionState state);
    void deviceReady(QObject *deviceRef);

    void testSignal();

private:
    bool isValidTransition(SessionState from, SessionState to) const;
    void setState(SessionState newState, const QString &errorText = QString());

    HWConnector *m_connector;
    DeviceBuilder *m_deviceCreator;
    // Non-owning guarded pointer: becomes nullptr automatically if deleted by owner.
    QPointer<DesktopDevice> m_device;
    SessionState m_state = SessionState::idle;
};

#endif // CORE_H
