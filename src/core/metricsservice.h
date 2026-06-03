#ifndef METRICSSERVICE_H
#define METRICSSERVICE_H

#include "dashboardmetricprofile.h"
#include "metricdescriptor.h"

#include <QObject>
#include <QString>
#include <QVariant>

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
    void metricUpdated(const QString &title,
                       Metrics::MetricId metricId,
                       const QVariant &value);

private:
    void discoverMetrics(const HardwareSnapshot &snapshot);
    void refreshMetricValues(const HardwareSnapshot &snapshot);

    bool m_metricsDiscovered = false;
    QList<MetricDescriptor> m_availableMetrics;
    DashboardMetricProfile m_metricProfile;
};

#endif // METRICSSERVICE_H
