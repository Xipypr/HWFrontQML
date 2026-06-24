#ifndef DASHBOARDMETRICPROFILE_H
#define DASHBOARDMETRICPROFILE_H

#include "metricdescriptor.h"

#include <hardwaresnapshot.h>

#include <QList>
#include <QHash>
#include <QStringList>

#include <optional>

struct DashboardMetricDefinition
{
    QString deviceId;
    HardwareKind hardwareKind = HardwareKind::Unknown;
    Metrics::MetricId metricId = Metrics::MetricId::Unknown;
    QString displayName;
    QString unit;
    bool showProgressBar = false;
};

class DashboardMetricProfile
{
public:
    struct SnapshotMetricIndex
    {
        struct MeasurementKey
        {
            QString deviceId;
            QString measurementId;
            MetricKind measurementKind = MetricKind::Unknown;

            bool operator==(const MeasurementKey &other) const;
        };

        QHash<QString, HardwareKind> deviceKindsById;
        QHash<MeasurementKey, const Measurement *> measurementsByKey;
    };

    SnapshotMetricIndex indexForSnapshot(const HardwareSnapshot &snapshot) const;

    QList<DashboardMetricDefinition> definitionsForSnapshot(const HardwareSnapshot &snapshot) const;
    QList<DashboardMetricDefinition> definitionsForSnapshot(const HardwareSnapshot &snapshot,
                                                            const SnapshotMetricIndex &index) const;

    std::optional<Measurement> measurementForDefinition(const HardwareSnapshot &snapshot,
                                                        const DashboardMetricDefinition &definition) const;

    std::optional<Measurement> measurementForDefinition(const SnapshotMetricIndex &index,
                                                        const DashboardMetricDefinition &definition) const;

private:
    struct MetricRule {
        Metrics::MetricId metricId = Metrics::MetricId::Unknown;
        MetricKind measurementKind = MetricKind::Unknown;
        QString metricLabel;
        QStringList sensorIdSuffixes;
        bool showProgressBar = false;
    };

    static QString displayNameForDeviceMetric(const HardwareDevice &device, const MetricRule &rule);
    QList<MetricRule> rulesForDeviceKind(HardwareKind kind) const;
};

inline bool DashboardMetricProfile::SnapshotMetricIndex::MeasurementKey::operator==(const MeasurementKey &other) const
{
    return deviceId == other.deviceId
            && measurementId == other.measurementId
            && measurementKind == other.measurementKind;
}

inline size_t qHash(const DashboardMetricProfile::SnapshotMetricIndex::MeasurementKey &key, size_t seed = 0) noexcept
{
    return qHashMulti(seed, key.deviceId, key.measurementId, static_cast<int>(key.measurementKind));
}

#endif // DASHBOARDMETRICPROFILE_H
