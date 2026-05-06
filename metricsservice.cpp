#include "metricsservice.h"

#include "devicebuilder.h"
#include "storages/desktopdevice.h"

#include <QHash>

namespace {
constexpr const char *LoadingMetricId = "loading";
constexpr const char *TemperatureMetricId = "temperature";
constexpr const char *FrequencyMetricId = "frequency";
constexpr const char *MetricUnitPercent = "%";
constexpr const char *MetricUnitCelsius = "°C";
constexpr const char *MetricUnitMegahertz = "MHz";

struct MetricDefinition
{
    const char *metricId;
    const char *displaySuffix;
    const char *unit;
};

const QList<MetricDefinition> &metricDefinitions()
{
    static const QList<MetricDefinition> definitions = {
        { LoadingMetricId, "Loading", MetricUnitPercent },
        { TemperatureMetricId, "Temperature", MetricUnitCelsius },
        { FrequencyMetricId, "Frequency", MetricUnitMegahertz }
    };

    return definitions;
}

QString metricUnit(const QString &metricId)
{
    for (const MetricDefinition &definition : metricDefinitions()) {
        if (metricId == QString::fromLatin1(definition.metricId))
            return QString::fromUtf8(definition.unit);
    }

    return {};
}
}


MetricsService::MetricsService(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<MetricDescriptor>("MetricDescriptor");
    qRegisterMetaType<QList<MetricDescriptor>>("QList<MetricDescriptor>");
}

QVariantList MetricsService::availableMetrics() const
{
    QVariantList result;
    for (const MetricDescriptor &descriptor : m_availableMetrics)
        result.push_back(descriptorToVariantMap(descriptor));

    return result;
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

        for (const MetricDefinition &definition : metricDefinitions()) {
            const QString metricId = QString::fromLatin1(definition.metricId);
            if (!hasMetric(deviceObject, metricId))
                continue;

            discoveredMetrics.push_back({
                deviceId,
                metricId,
                metricDisplayName(deviceObject, deviceId),
                metricUnit(metricId)
            });
        }
    }

    m_availableMetrics = discoveredMetrics;
    m_metricsDiscovered = true;
    emit availableMetricsChanged(availableMetrics());
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

bool MetricsService::hasMetric(Device *deviceObject, const QString &metricId)
{
    if (!deviceObject)
        return false;

    return deviceObject->property(metricId.toLatin1().constData()).isValid();
}

QVariantMap MetricsService::descriptorToVariantMap(const MetricDescriptor &descriptor)
{
    return {
        { QStringLiteral("deviceId"), descriptor.deviceId },
        { QStringLiteral("metricId"), descriptor.metricId },
        { QStringLiteral("displayName"), descriptor.displayName },
        { QStringLiteral("unit"), descriptor.unit }
    };
}

QVariant MetricsService::metricValue(Device *deviceObject, const QString &metricId) const
{
    if (!deviceObject)
        return {};

    return deviceObject->property(metricId.toLatin1().constData());
}
