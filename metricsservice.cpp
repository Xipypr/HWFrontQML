#include "metricsservice.h"

#include "devicebuilder.h"
#include "storages/desktopdevice.h"

#include <QHash>

namespace {
const QList<Metrics::MetricId> &metricDefinitions()
{
    static const QList<Metrics::MetricId> definitions = {
        Metrics::MetricId::Loading,
        Metrics::MetricId::Temperature,
        Metrics::MetricId::Frequency
    };

    return definitions;
}
}


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
    QList<MetricDescriptor> discoveredMetrics;

    const QList<Device *> devices = desktopDevice->devicesList();
    for (Device *deviceObject : devices) {
        if (!deviceObject)
            continue;

        const QString deviceId = metricDeviceId(deviceObject);
        if (deviceId.isEmpty())
            continue;

        for (Metrics::MetricId metricId : metricDefinitions()) {
            if (!hasMetric(deviceObject, metricId))
                continue;

            discoveredMetrics.push_back({
                deviceId,
                metricId,
                metricDisplayName(deviceObject, deviceId),
                Metrics::metricUnit(metricId)
            });
        }
    }

    m_availableMetrics = discoveredMetrics;
    m_metricsDiscovered = true;
    emit availableMetricsChanged(m_availableMetrics);
}

void MetricsService::refreshMetricValues(DesktopDevice *desktopDevice)
{
    QHash<QString, Device *> devicesById;

    const QList<Device *> devices = desktopDevice->devicesList();
    for (Device *deviceObject : devices) {
        const QString deviceId = metricDeviceId(deviceObject);
        if (!deviceId.isEmpty() && !devicesById.contains(deviceId))
            devicesById.insert(deviceId, deviceObject);
    }

    for (const MetricDescriptor &descriptor : m_availableMetrics) {
        Device *deviceObject = devicesById.value(descriptor.deviceId, nullptr);
        if (!deviceObject)
            continue;

        emit metricUpdated(descriptor.deviceId,
                           descriptor.metricId,
                           metricValue(deviceObject, descriptor.metricId));
    }
}


QString MetricsService::metricDeviceId(Device *deviceObject)
{
    if (!deviceObject)
        return {};

    switch (deviceObject->type()) {
    case Device::PROCESSOR:
        return QStringLiteral("cpu");
    case Device::MEMORY:
        return QStringLiteral("ram");
    case Device::VIDEO_CARD:
        return QStringLiteral("gpu");
    case Device::HARD_DISK:
        return QStringLiteral("hdd");
    default:
        return {};
    }
}


QString MetricsService::metricDisplayName(Device *deviceObject, const QString &deviceId)
{
    const QString fallbackName = deviceId.toUpper();
    const QString deviceName = deviceObject ? deviceObject->property("name").toString() : QString();
    return deviceName.isEmpty() ? fallbackName : deviceName;
}

bool MetricsService::hasMetric(Device *deviceObject, Metrics::MetricId metricId)
{
    if (!deviceObject)
        return false;

    const QString metricName = Metrics::metricIdToString(metricId);
    return metricId != Metrics::MetricId::Unknown
           && deviceObject->property(metricName.toLatin1().constData()).isValid();
}

QVariant MetricsService::metricValue(Device *deviceObject, Metrics::MetricId metricId) const
{
    if (!deviceObject)
        return {};

    if (metricId == Metrics::MetricId::Unknown)
        return {};

    const QString metricName = Metrics::metricIdToString(metricId);
    return deviceObject->property(metricName.toLatin1().constData());
}
