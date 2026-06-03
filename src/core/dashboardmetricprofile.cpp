#include "dashboardmetricprofile.h"

#include <hardwaredevice.h>

QList<DashboardMetricDefinition> DashboardMetricProfile::definitionsForSnapshot(const HardwareSnapshot &snapshot) const
{
    QList<DashboardMetricDefinition> definitions;

    for (const HardwareDevice &device : snapshot.devices) {
        if (device.id.isEmpty())
            continue;

        for (const MetricRule &rule : rulesForDeviceKind(device.kind)) {
            DashboardMetricDefinition definition;
            definition.deviceId = device.id;
            definition.metricId = rule.metricId;
            definition.displayName = rule.displayName.isEmpty() ? device.name : rule.displayName;
            definition.unit = rule.unit;
            definition.showProgressBar = rule.showProgressBar;

            if (measurementForDefinition(snapshot, definition).has_value())
                definitions.append(definition);
        }
    }

    return definitions;
}

std::optional<Measurement> DashboardMetricProfile::measurementForDefinition(
    const HardwareSnapshot &snapshot,
    const DashboardMetricDefinition &definition) const
{
    if (definition.deviceId.isEmpty() || definition.metricId == Metrics::MetricId::Unknown)
        return std::nullopt;

    HardwareKind deviceKind = HardwareKind::Unknown;
    for (const HardwareDevice &device : snapshot.devices) {
        if (device.id == definition.deviceId) {
            deviceKind = device.kind;
            break;
        }
    }

    const QList<MetricRule> rules = rulesForDeviceKind(deviceKind);
    for (const MetricRule &rule : rules) {
        if (rule.metricId != definition.metricId)
            continue;

        QStringList sensorIds;
        for (const QString &suffix : rule.sensorIdSuffixes)
            sensorIds.append(definition.deviceId + suffix);

        for (const QString &sensorId : sensorIds) {
            for (const Measurement &measurement : snapshot.measurements) {
                if (measurement.id == sensorId
                        && measurement.deviceId == definition.deviceId
                        && measurement.kind == rule.measurementKind
                        && measurement.value.has_value()) {
                    return measurement;
                }
            }
        }

        return std::nullopt;
    }

    return std::nullopt;
}

QList<DashboardMetricProfile::MetricRule> DashboardMetricProfile::rulesForDeviceKind(HardwareKind kind) const
{
    switch (kind) {
    case HardwareKind::Cpu:
        return {
            {
                Metrics::MetricId::Loading,
                MetricKind::Load,
                QStringLiteral("CPU Loading"),
                { QStringLiteral("/load/0") },
                true,
                Metrics::metricUnit(Metrics::MetricId::Loading)
            },
            {
                Metrics::MetricId::Temperature,
                MetricKind::Temperature,
                QStringLiteral("CPU Temperature"),
                { QStringLiteral("/temperature/2") },
                false,
                Metrics::metricUnit(Metrics::MetricId::Temperature)
            },
            {
                Metrics::MetricId::Frequency,
                MetricKind::Clock,
                QStringLiteral("CPU Frequency"),
                { QStringLiteral("/clock/1") },
                false,
                Metrics::metricUnit(Metrics::MetricId::Frequency)
            }
        };
    case HardwareKind::Gpu:
        return {
            {
                Metrics::MetricId::Loading,
                MetricKind::Load,
                QStringLiteral("GPU Loading"),
                { QStringLiteral("/load/0") },
                true,
                Metrics::metricUnit(Metrics::MetricId::Loading)
            },
            {
                Metrics::MetricId::Temperature,
                MetricKind::Temperature,
                QStringLiteral("GPU Temperature"),
                { QStringLiteral("/temperature/0") },
                false,
                Metrics::metricUnit(Metrics::MetricId::Temperature)
            }
        };
    case HardwareKind::Memory:
        return {
            {
                Metrics::MetricId::Loading,
                MetricKind::Load,
                QStringLiteral("RAM Loading"),
                { QStringLiteral("/load/0") },
                true,
                Metrics::metricUnit(Metrics::MetricId::Loading)
            }
        };
    case HardwareKind::Storage:
        return {
            {
                Metrics::MetricId::Loading,
                MetricKind::Load,
                QStringLiteral("Storage Loading"),
                { QStringLiteral("/load/30") },
                true,
                Metrics::metricUnit(Metrics::MetricId::Loading)
            },
            {
                Metrics::MetricId::Temperature,
                MetricKind::Temperature,
                QStringLiteral("Storage Temperature"),
                { QStringLiteral("/temperature/0") },
                false,
                Metrics::metricUnit(Metrics::MetricId::Temperature)
            }
        };
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
