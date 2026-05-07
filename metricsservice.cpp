#include "metricsservice.h"

#include "devicebuilder.h"
#include "storages/desktopdevice.h"

#include <QHash>

namespace {
constexpr const char *LoadingPropertyName = "loading";
constexpr const char *TemperaturePropertyName = "temperature";
constexpr const char *FrequencyPropertyName = "frequency";
constexpr const char *MetricUnitPercent = "%";
constexpr const char *MetricUnitCelsius = "°C";
constexpr const char *MetricUnitMegahertz = "MHz";

struct MetricDefinition
{
    Metrics::MetricId metricId;
    const char *propertyName;
    const char *displaySuffix;
    const char *unit;
};

const QList<MetricDefinition> &metricDefinitions()
{
    static const QList<MetricDefinition> definitions = {
        { Metrics::MetricId::Loading, LoadingPropertyName, "Loading", MetricUnitPercent },
        { Metrics::MetricId::Temperature, TemperaturePropertyName, "Temperature", MetricUnitCelsius },
        { Metrics::MetricId::Frequency, FrequencyPropertyName, "Frequency", MetricUnitMegahertz }
    };

    return definitions;
}

const MetricDefinition *metricDefinition(Metrics::MetricId metricId)
{
    for (const MetricDefinition &definition : metricDefinitions()) {
        if (definition.metricId == metricId)
            return &definition;
    }

    return nullptr;
}

QString metricPropertyName(Metrics::MetricId metricId)
{
    const MetricDefinition *definition = metricDefinition(metricId);
    return definition ? QString::fromLatin1(definition->propertyName) : QString();
}

QString metricUnit(Metrics::MetricId metricId)
{
    const MetricDefinition *definition = metricDefinition(metricId);
    return definition ? QString::fromUtf8(definition->unit) : QString();
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

        for (const MetricDefinition &definition : metricDefinitions()) {
            if (!hasMetric(deviceObject, definition.metricId))
                continue;

            discoveredMetrics.push_back({
                deviceId,
                definition.metricId,
                metricDisplayName(deviceObject, deviceId),
                metricUnit(definition.metricId)
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

    const QString propertyName = metricPropertyName(metricId);
    return !propertyName.isEmpty() && deviceObject->property(propertyName.toLatin1().constData()).isValid();
}

QVariant MetricsService::metricValue(Device *deviceObject, Metrics::MetricId metricId) const
{
    if (!deviceObject)
        return {};

    const QString propertyName = metricPropertyName(metricId);
    if (propertyName.isEmpty())
        return {};

    return deviceObject->property(propertyName.toLatin1().constData());
}
