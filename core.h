#ifndef CORE_H
#define CORE_H

#include "hwconnector.h"
#include "devicebuilder.h"

#include <QObject>

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(QObject *parent = nullptr);

public slots:
    void onStartMonitoring();
    void onMakeGetRequest(const QString & target);

signals:
    void deviceCreated(DesktopDevice * device);

    void testSignal();

private:
    HWConnector * m_connector;
    DeviceBuilder * m_deviceCreator;

};

#endif // CORE_H
