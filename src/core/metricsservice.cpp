#include "metricsservice.h"

#include <hardwaresnapshot.h>

namespace {

double dashboardMetricValue(Metrics::MetricId metricId, const Measurement &measurement)
{
    const double value = measurement.value.value_or(0.0f);

    if (metricId != Metrics::MetricId::NetworkUpload
            && metricId != Metrics::MetricId::NetworkDownload) {
        return value;
    }

    switch (measurement.unit) {
    case Unit::MegabytePerSecond:
        return value;
    case Unit::KilobytePerSecond:
        return value / 1024.0;
    default:
        return value / (1024.0 * 1024.0);
    }
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

void MetricsService::processSnapshot(const HardwareSnapshot &snapshot)
{
    if (!m_metricsDiscovered)
        discoverMetrics(snapshot);

    refreshMetricValues(snapshot);
}

void MetricsService::discoverMetrics(const HardwareSnapshot &snapshot)
{
    m_availableMetrics.clear();

    for (const DashboardMetricDefinition &definition : m_metricProfile.definitionsForSnapshot(snapshot)) {
        m_availableMetrics.append({
            definition.deviceId,
            definition.metricId,
            definition.displayName,
            definition.unit,
            definition.showProgressBar
        });
    }

    m_metricsDiscovered = true;
    emit availableMetricsChanged(m_availableMetrics);
}

void MetricsService::refreshMetricValues(const HardwareSnapshot &snapshot)
{
    for (const MetricDescriptor &descriptor : m_availableMetrics) {
        const DashboardMetricDefinition definition {
            descriptor.deviceId,
            descriptor.metricId,
            descriptor.displayName,
            descriptor.unit,
            descriptor.showProgressBar
        };
        const std::optional<Measurement> measurement = m_metricProfile.measurementForDefinition(snapshot, definition);
        if (!measurement.has_value())
            continue;

        emit metricUpdated(descriptor.deviceId,
                           descriptor.metricId,
                           dashboardMetricValue(descriptor.metricId, measurement.value()));
    }
}
