#ifndef CORE_H
#define CORE_H

#include "hwconnector.h"
#include "devicebuilder.h"

#include <QObject>
#include <QPointer>

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core();
    ~Core();

    Q_INVOKABLE QObject *device() const;
public slots:
    void onStartMonitoring();
    void onMakeGetRequest(const QString &sessionId, const QString &target);
    void onRemoveSession(const QString &sessionId);

    void onDeviceCreated(DesktopDevice *device);

signals:
    void sessionAdded(const QString &sessionId);
    void sessionStateChanged(const QString &sessionId, const QString &state);
    void deviceReady(const QString &sessionId, QObject *deviceRef);
    void sessionRemoved(const QString &sessionId);

    void testSignal();

private:
    HWConnector *m_connector;
    DeviceBuilder *m_deviceCreator;
    // Non-owning guarded pointer: becomes nullptr automatically if deleted by owner.
    QPointer<DesktopDevice> m_device;
    QString m_pendingSessionId;
    QString m_activeSessionId;
};

#endif // CORE_H
