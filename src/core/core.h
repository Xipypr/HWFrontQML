#ifndef CORE_H
#define CORE_H

#include <hardwaresnapshot.h>
#include "hwconnector.h"
#include "sessionstate.h"

#include <QObject>
#include <QJsonObject>

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(QObject *parent = nullptr);
    ~Core();

public slots:
    void onStartConnection(const QString &target);
    void onCloseConnection();
    void onLhmDocumentReceived(const QJsonObject &document);
    void onLinuxDocumentReceived(const QJsonObject &document);
    void onStatusChanged(HWConnector::ConnectionStatus status);

signals:
    void sessionStateChanged(SessionState state);
    void snapshotReady(const HardwareSnapshot &snapshot, const QString &displayName);

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

        return SessionState::ERROR;
    }

    bool isValidTransition(SessionState from, SessionState to) const;
    void setState(SessionState newState);
    static QString displayNameFromDocument(const QJsonObject &document);

    HWConnector *m_connector;
    SessionState m_state = SessionState::IDLE;
};

#endif // CORE_H
