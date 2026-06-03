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
            definition.displayName = displayNameForDeviceMetric(device, rule);
            definition.unit = Metrics::metricUnit(rule.metricId);
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

QString DashboardMetricProfile::displayNameForDeviceMetric(const HardwareDevice &device, const MetricRule &rule)
{
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
    case HardwareKind::Unknown:
    case HardwareKind::Computer:
    case HardwareKind::Motherboard:
    case HardwareKind::MemoryModule:
    case HardwareKind::Network:
        return {};
    }

    return {};
}
