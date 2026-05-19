#include "metricsservice.h"

#include "devicemetricfactory.h"
#include "storages/desktopdevice.h"

#include <QHash>


MetricsService::MetricsService(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<Metrics::MetricId>("Metrics::MetricId");
    qRegisterMetaType<MetricDescriptor>("MetricDescriptor");
    qRegisterMetaType<QList<MetricDescriptor>>("QList<MetricDescriptor>");
}

QList<MetricDescriptor> MetricsService::metricDescriptors() const
{
    return m_availableMetrics;
}

void MetricsService::processDeviceSnapshot(DesktopDevice *desktopDevice)
{
    if (!desktopDevice)
        return;

    if (!m_metricsDiscovered)
        discoverMetrics(desktopDevice);

    refreshMetricValues(desktopDevice);
}

void MetricsService::discoverMetrics(DesktopDevice *desktopDevice)
{
    m_availableMetrics = DeviceMetricFactory::createDescriptors(desktopDevice);
    m_metricsDiscovered = true;
    emit availableMetricsChanged(m_availableMetrics);
}

void MetricsService::refreshMetricValues(DesktopDevice *desktopDevice)
{
    QHash<QString, Device *> devicesById;

    for (Device *deviceObject : desktopDevice->devicesList())
    {
        const QString deviceId = DeviceMetricFactory::deviceId(deviceObject);
        if (!deviceId.isEmpty() && !devicesById.contains(deviceId))
            devicesById.insert(deviceId, deviceObject);
    }

    for (const MetricDescriptor &descriptor : m_availableMetrics)
    {
        Device *deviceObject = devicesById.value(descriptor.deviceId, nullptr);
        if (!deviceObject)
            continue;

        emit metricUpdated(descriptor.displayName,
                           descriptor.metricId,
                           DeviceMetricFactory::metricValue(deviceObject, descriptor.metricId));
    }
}
