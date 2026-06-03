#include "metricsservice.h"

#include "devicemetricfactory.h"
#include "storages/desktopdevice.h"

#include <hardwaresnapshot.h>

#include <QHash>
#include <QStringList>

#include <optional>

namespace {

struct SnapshotMetricRef {
    QString deviceId;
    Metrics::MetricId metricId = Metrics::MetricId::Unknown;
    QStringList preferredNames;
};

bool isDashboardDeviceKind(HardwareKind kind)
{
    switch (kind) {
    case HardwareKind::Cpu:
    case HardwareKind::Memory:
    case HardwareKind::Gpu:
    case HardwareKind::Storage:
        return true;
    case HardwareKind::Unknown:
    case HardwareKind::Computer:
    case HardwareKind::Motherboard:
    case HardwareKind::MemoryModule:
    case HardwareKind::Network:
    case HardwareKind::Battery:
        return false;
    }

    return false;
}

MetricKind snapshotMetricKind(Metrics::MetricId metricId)
{
    switch (metricId) {
    case Metrics::MetricId::Loading:
        return MetricKind::Load;
    case Metrics::MetricId::Temperature:
        return MetricKind::Temperature;
    case Metrics::MetricId::Frequency:
        return MetricKind::Clock;
    case Metrics::MetricId::Unknown:
        return MetricKind::Unknown;
    }

    return MetricKind::Unknown;
}

QStringList preferredMeasurementNames(HardwareKind deviceKind, Metrics::MetricId metricId)
{
    if (metricId == Metrics::MetricId::Loading) {
        switch (deviceKind) {
        case HardwareKind::Cpu:
            return { QStringLiteral("CPU Total"), QStringLiteral("Total") };
        case HardwareKind::Gpu:
            return { QStringLiteral("GPU Core"), QStringLiteral("D3D 3D"), QStringLiteral("Total") };
        case HardwareKind::Memory:
            return { QStringLiteral("Memory"), QStringLiteral("Memory Used") };
        case HardwareKind::Storage:
            return { QStringLiteral("Used Space"), QStringLiteral("Total Activity") };
        default:
            return {};
        }
    }

    if (metricId == Metrics::MetricId::Temperature) {
        switch (deviceKind) {
        case HardwareKind::Cpu:
            return { QStringLiteral("Core (Tctl/Tdie)"), QStringLiteral("CPU Package"), QStringLiteral("Package"), QStringLiteral("Core Average") };
        case HardwareKind::Gpu:
            return { QStringLiteral("GPU Core"), QStringLiteral("Temperature") };
        case HardwareKind::Storage:
            return { QStringLiteral("Temperature") };
        default:
            return {};
        }
    }

    return {};
}

QList<Metrics::MetricId> dashboardMetricIds(HardwareKind deviceKind)
{
    switch (deviceKind) {
    case HardwareKind::Cpu:
    case HardwareKind::Gpu:
        return { Metrics::MetricId::Loading, Metrics::MetricId::Temperature };
    case HardwareKind::Memory:
    case HardwareKind::Storage:
        return { Metrics::MetricId::Loading };
    case HardwareKind::Unknown:
    case HardwareKind::Computer:
    case HardwareKind::Motherboard:
    case HardwareKind::MemoryModule:
    case HardwareKind::Network:
    case HardwareKind::Battery:
        return {};
    }

    return {};
}

bool nameContainsAny(const QString &name, const QStringList &parts)
{
    for (const QString &part : parts) {
        if (name.contains(part, Qt::CaseInsensitive))
            return true;
    }
    return false;
}

std::optional<Measurement> findPreferredMeasurement(const HardwareSnapshot &snapshot,
                                                    const SnapshotMetricRef &metricRef)
{
    QList<Measurement> candidates;
    const MetricKind metricKind = snapshotMetricKind(metricRef.metricId);

    for (const Measurement &measurement : snapshot.measurements) {
        if (measurement.deviceId == metricRef.deviceId
                && measurement.kind == metricKind
                && measurement.value.has_value()) {
            candidates.append(measurement);
        }
    }

    for (const QString &preferredName : metricRef.preferredNames) {
        for (const Measurement &candidate : candidates) {
            if (candidate.name.compare(preferredName, Qt::CaseInsensitive) == 0)
                return candidate;
        }
    }

    for (const Measurement &candidate : candidates) {
        if (nameContainsAny(candidate.name, metricRef.preferredNames))
            return candidate;
    }

    if (!candidates.isEmpty())
        return candidates.first();

    return std::nullopt;
}

} // namespace


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

void MetricsService::processSnapshot(const HardwareSnapshot &snapshot)
{
    if (!m_metricsDiscovered)
        discoverMetrics(snapshot);

    refreshMetricValues(snapshot);
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

void MetricsService::discoverMetrics(const HardwareSnapshot &snapshot)
{
    m_availableMetrics.clear();

    for (const HardwareDevice &device : snapshot.devices) {
        if (!isDashboardDeviceKind(device.kind))
            continue;

        if (device.id.isEmpty())
            continue;

        for (Metrics::MetricId metricId : dashboardMetricIds(device.kind)) {
            const SnapshotMetricRef metricRef {
                device.id,
                metricId,
                preferredMeasurementNames(device.kind, metricId)
            };

            if (!findPreferredMeasurement(snapshot, metricRef).has_value())
                continue;

            m_availableMetrics.append({
                device.id,
                metricId,
                device.name,
                Metrics::metricUnit(metricId)
            });
        }
    }

    m_metricsDiscovered = true;
    emit availableMetricsChanged(m_availableMetrics);
}

void MetricsService::refreshMetricValues(const HardwareSnapshot &snapshot)
{
    QHash<QString, HardwareDevice> devicesById;

    for (const HardwareDevice &device : snapshot.devices) {
        if (!device.id.isEmpty() && !devicesById.contains(device.id))
            devicesById.insert(device.id, device);
    }

    for (const MetricDescriptor &descriptor : m_availableMetrics) {
        const HardwareDevice device = devicesById.value(descriptor.deviceId);
        if (device.id.isEmpty())
            continue;

        const SnapshotMetricRef metricRef {
            device.id,
            descriptor.metricId,
            preferredMeasurementNames(device.kind, descriptor.metricId)
        };
        const std::optional<Measurement> measurement = findPreferredMeasurement(snapshot, metricRef);
        if (!measurement.has_value())
            continue;

        emit metricUpdated(descriptor.displayName,
                           descriptor.metricId,
                           QVariant::fromValue(measurement.value().value.value_or(0.0f)));
    }
}
