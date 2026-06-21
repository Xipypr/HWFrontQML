#ifndef METRICSSERVICE_H
#define METRICSSERVICE_H

#include "dashboardmetricprofile.h"
#include "metricdescriptor.h"

#include <QObject>
#include <QString>

struct HardwareSnapshot;

class MetricsService : public QObject
{
    Q_OBJECT
public:
    explicit MetricsService(QObject *parent = nullptr);

    QList<MetricDescriptor> metricDescriptors() const;

public slots:
    void processSnapshot(const HardwareSnapshot &snapshot);

signals:
    void availableMetricsChanged(const QList<MetricDescriptor> &metrics);
    void metricUpdated(const QString &deviceId,
                       Metrics::MetricId metricId,
                       double value);

private:
    void discoverMetrics(const HardwareSnapshot &snapshot);
    void refreshMetricValues(const HardwareSnapshot &snapshot);

    bool m_metricsDiscovered = false;
    QList<MetricDescriptor> m_availableMetrics;
    DashboardMetricProfile m_metricProfile;
};

#endif // METRICSSERVICE_H
