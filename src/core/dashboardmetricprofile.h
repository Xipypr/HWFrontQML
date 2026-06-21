#ifndef DASHBOARDMETRICPROFILE_H
#define DASHBOARDMETRICPROFILE_H

#include "metricdescriptor.h"

#include <hardwaresnapshot.h>

#include <QList>
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
    QList<DashboardMetricDefinition> definitionsForSnapshot(const HardwareSnapshot &snapshot) const;
    std::optional<Measurement> measurementForDefinition(
        const HardwareSnapshot &snapshot,
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

#endif // DASHBOARDMETRICPROFILE_H
