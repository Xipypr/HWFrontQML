#include "dashboardmetricprofile.h"

#include <hardwaredevice.h>

namespace {

bool nameContainsAny(const QString &name, const QStringList &parts)
{
    for (const QString &part : parts) {
        if (name.contains(part, Qt::CaseInsensitive))
            return true;
    }
    return false;
}

} // namespace

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

        QList<Measurement> candidates;
        for (const Measurement &measurement : snapshot.measurements) {
            if (measurement.deviceId == definition.deviceId
                    && measurement.kind == rule.measurementKind
                    && measurement.value.has_value()) {
                candidates.append(measurement);
            }
        }

        for (const QString &primaryName : rule.primaryNames) {
            for (const Measurement &candidate : candidates) {
                if (candidate.name.compare(primaryName, Qt::CaseInsensitive) == 0)
                    return candidate;
            }
        }

        for (const Measurement &candidate : candidates) {
            if (nameContainsAny(candidate.name, rule.primaryNames))
                return candidate;
        }

        if (rule.allowFirstMatchFallback && !candidates.isEmpty())
            return candidates.first();

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
                { QStringLiteral("CPU Total"), QStringLiteral("Total") },
                true,
                true,
                Metrics::metricUnit(Metrics::MetricId::Loading)
            },
            {
                Metrics::MetricId::Temperature,
                MetricKind::Temperature,
                QStringLiteral("CPU Temperature"),
                { QStringLiteral("Core (Tctl/Tdie)"), QStringLiteral("CPU Package"), QStringLiteral("Package"), QStringLiteral("Core Average") },
                true,
                false,
                Metrics::metricUnit(Metrics::MetricId::Temperature)
            },
            {
                Metrics::MetricId::Frequency,
                MetricKind::Clock,
                QStringLiteral("CPU Frequency"),
                { QStringLiteral("Cores (Average)"), QStringLiteral("Core Average") },
                true,
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
                { QStringLiteral("GPU Core"), QStringLiteral("D3D 3D"), QStringLiteral("Total") },
                true,
                true,
                Metrics::metricUnit(Metrics::MetricId::Loading)
            },
            {
                Metrics::MetricId::Temperature,
                MetricKind::Temperature,
                QStringLiteral("GPU Temperature"),
                { QStringLiteral("GPU Core"), QStringLiteral("Temperature") },
                true,
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
                { QStringLiteral("Memory"), QStringLiteral("Memory Used") },
                true,
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
                { QStringLiteral("Used Space"), QStringLiteral("Total Activity") },
                true,
                true,
                Metrics::metricUnit(Metrics::MetricId::Loading)
            },
            {
                Metrics::MetricId::Temperature,
                MetricKind::Temperature,
                QStringLiteral("Storage Temperature"),
                { QStringLiteral("Temperature") },
                true,
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
