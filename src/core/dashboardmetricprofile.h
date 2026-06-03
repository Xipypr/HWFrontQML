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
        QString displayName;
        QStringList primaryNames;
        bool allowFirstMatchFallback = false;
        bool showProgressBar = false;
        QString unit;
    };

    QList<MetricRule> rulesForDeviceKind(HardwareKind kind) const;
};

#endif // DASHBOARDMETRICPROFILE_H
