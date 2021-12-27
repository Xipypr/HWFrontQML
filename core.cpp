#include "core.h"

#include <QDebug>

Core::Core(QObject *parent) : QObject(parent)
{
    m_connector = new HWConnector("KEK", this);
    m_deviceCreator = new DeviceBuilder(this);
    connect( m_connector, &HWConnector::documentRecieved, m_deviceCreator, &DeviceBuilder::onDocumentRecieved );
    connect( m_deviceCreator, &DeviceBuilder::desktopCreated, this, &Core::deviceCreated );
    connect( m_deviceCreator, &DeviceBuilder::testSignal, this, &Core::testSignal );
    //    connector->makeGetRequest();
}

void Core::onStartMonitoring()
{

}

void Core::onMakeGetRequest(const QString &target)
{
    m_connector->makeGetRequest(target);
}
