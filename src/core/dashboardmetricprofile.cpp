#include "dashboardmetricprofile.h"

#include <hardwaredevice.h>

DashboardMetricProfile::SnapshotMetricIndex DashboardMetricProfile::indexForSnapshot(
    const HardwareSnapshot &snapshot) const
{
    SnapshotMetricIndex index;

    for (const HardwareDevice &device : snapshot.devices) {
        if (!index.deviceKindsById.contains(device.id))
            index.deviceKindsById.insert(device.id, device.kind);
    }

    for (const Measurement &measurement : snapshot.measurements) {
        if (!measurement.value.has_value())
            continue;

        const SnapshotMetricIndex::MeasurementKey key {
            measurement.deviceId,
            measurement.id,
            measurement.kind
        };

        if (!index.measurementsByKey.contains(key))
            index.measurementsByKey.insert(key, &measurement);
    }

    return index;
}

QList<DashboardMetricDefinition> DashboardMetricProfile::definitionsForSnapshot(const HardwareSnapshot &snapshot) const
{
    QList<DashboardMetricDefinition> definitions;
    const SnapshotMetricIndex index = indexForSnapshot(snapshot);

    for (const HardwareDevice &device : snapshot.devices) {
        if (device.id.isEmpty())
            continue;

        for (const MetricRule &rule : rulesForDeviceKind(device.kind)) {
            DashboardMetricDefinition definition;
            definition.deviceId = device.id;
            definition.hardwareKind = device.kind;
            definition.metricId = rule.metricId;
            definition.displayName = displayNameForDeviceMetric(device, rule);
            definition.unit = Metrics::metricUnit(rule.metricId);
            definition.showProgressBar = rule.showProgressBar;

            if (measurementForDefinition(index, definition).has_value())
                definitions.append(definition);
        }
    }

    return definitions;
}

std::optional<Measurement> DashboardMetricProfile::measurementForDefinition(
    const HardwareSnapshot &snapshot,
    const DashboardMetricDefinition &definition) const
{
    return measurementForDefinition(indexForSnapshot(snapshot), definition);
}

std::optional<Measurement> DashboardMetricProfile::measurementForDefinition(
    const SnapshotMetricIndex &index,
    const DashboardMetricDefinition &definition) const
{
    if (definition.deviceId.isEmpty() || definition.metricId == Metrics::MetricId::Unknown)
        return std::nullopt;

    HardwareKind deviceKind = HardwareKind::Unknown;
    const auto deviceKindIt = index.deviceKindsById.constFind(definition.deviceId);
    if (deviceKindIt != index.deviceKindsById.constEnd())
        deviceKind = deviceKindIt.value();

    const QList<MetricRule> rules = rulesForDeviceKind(deviceKind);
    for (const MetricRule &rule : rules) {
        if (rule.metricId != definition.metricId)
            continue;

        QStringList sensorIds;
        for (const QString &suffix : rule.sensorIdSuffixes)
            sensorIds.append(definition.deviceId + suffix);

        for (const QString &sensorId : sensorIds) {
            const SnapshotMetricIndex::MeasurementKey key {
                definition.deviceId,
                sensorId,
                rule.measurementKind
            };
            const auto measurementIt = index.measurementsByKey.constFind(key);
            if (measurementIt != index.measurementsByKey.constEnd())
                return *measurementIt.value();
        }

        return std::nullopt;
    }

    return std::nullopt;
}

QString DashboardMetricProfile::displayNameForDeviceMetric(const HardwareDevice &device, const MetricRule &rule)
{
    if (device.kind == HardwareKind::Battery)
        return QStringLiteral("Battery");

    const QString deviceName = device.name.trimmed().isEmpty() ? device.id : device.name.trimmed();
    const QString metricLabel = rule.metricLabel.trimmed();

    if (deviceName.isEmpty())
        return metricLabel;

    if (metricLabel.isEmpty())
        return deviceName;

    return deviceName + QStringLiteral(" ") + metricLabel;
}

QList<DashboardMetricProfile::MetricRule> DashboardMetricProfile::rulesForDeviceKind(HardwareKind kind) const
{
    switch (kind) {
    case HardwareKind::Cpu:
        return {
            {
                Metrics::MetricId::Loading,
                MetricKind::Load,
                QStringLiteral("Loading"),
                { QStringLiteral("/load/0") },
                true
            },
            {
                Metrics::MetricId::Temperature,
                MetricKind::Temperature,
                QStringLiteral("Temperature"),
                { QStringLiteral("/temperature/2") },
                false
            },
            {
                Metrics::MetricId::Frequency,
                MetricKind::Clock,
                QStringLiteral("Frequency"),
                { QStringLiteral("/clock/1") },
                false
            }
        };
    case HardwareKind::Gpu:
        return {
            {
                Metrics::MetricId::Loading,
                MetricKind::Load,
                QStringLiteral("Loading"),
                { QStringLiteral("/load/0") },
                true
            },
            {
                Metrics::MetricId::Temperature,
                MetricKind::Temperature,
                QStringLiteral("Temperature"),
                { QStringLiteral("/temperature/0") },
                false
            }
        };
    case HardwareKind::Memory:
        return {
            {
                Metrics::MetricId::Loading,
                MetricKind::Load,
                QStringLiteral("Loading"),
                { QStringLiteral("/load/0") },
                true
            }
        };
    case HardwareKind::Storage:
        return {
            {
                Metrics::MetricId::Loading,
                MetricKind::Load,
                QStringLiteral("Used Space"),
                { QStringLiteral("/load/30") },
                true
            },
            {
                Metrics::MetricId::Temperature,
                MetricKind::Temperature,
                QStringLiteral("Temperature"),
                { QStringLiteral("/temperature/0") },
                false
            }
        };
    case HardwareKind::Battery:
        return {
            {
                Metrics::MetricId::BatteryLevel,
                MetricKind::Level,
                QStringLiteral("Level"),
                { QStringLiteral("/level/0") },
                true
            }
        };
    case HardwareKind::Network:
        return {
            {
                Metrics::MetricId::NetworkDownload,
                MetricKind::Throughput,
                QString(),
                { QStringLiteral("/throughput/8") },
                false
            },
            {
                Metrics::MetricId::NetworkUpload,
                MetricKind::Throughput,
                QStringLiteral("Upload"),
                { QStringLiteral("/throughput/7") },
                false
            }
        };
    case HardwareKind::Unknown:
    case HardwareKind::Computer:
    case HardwareKind::Motherboard:
    case HardwareKind::MemoryModule:
        return {};
    }

    return {};
}
