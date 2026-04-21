#ifndef CORE_H
#define CORE_H

#include "hwconnector.h"
#include "devicebuilder.h"
#include "session.h"

#include <QObject>
#include <QPointer>

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core();
    ~Core();

    Q_INVOKABLE QObject *device() const;
    Q_INVOKABLE QString sessionId() const;
public slots:
    void onStartMonitoring();
    void onMakeGetRequest(const QString &target);
    void onDeviceCreated(DesktopDevice *device);
    void onConnectorError(const QString &errorText);
    void onConnectorDisconnected();

signals:
    void sessionStateChanged(const QString &sessionId, const QString &state);
    void deviceReady(const QString &sessionId, QObject *deviceRef);

    void testSignal();

private:
    bool isValidTransition(SessionState from, SessionState to) const;
    void setState(SessionState newState, const QString &errorText = QString());

    HWConnector *m_connector;
    DeviceBuilder *m_deviceCreator;
    // Non-owning guarded pointer: becomes nullptr automatically if deleted by owner.
    QPointer<DesktopDevice> m_device;
    Session m_session;
};

#endif // CORE_H
