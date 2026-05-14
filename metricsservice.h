#ifndef METRICSSERVICE_H
#define METRICSSERVICE_H

#include "metricdescriptor.h"

#include <QObject>
#include <QString>
#include <QVariant>

class DesktopDevice;
class Device;

class MetricsService : public QObject
{
    Q_OBJECT
public:
    explicit MetricsService(QObject *parent = nullptr);

    QList<MetricDescriptor> metricDescriptors() const;

public slots:
    void processDeviceSnapshot(DesktopDevice *desktopDevice);

signals:
    void availableMetricsChanged(const QList<MetricDescriptor> &metrics);
    void metricUpdated(const QString &title,
                       Metrics::MetricId metricId,
                       const QVariant &value);

private:
    void discoverMetrics(DesktopDevice *desktopDevice);
    void refreshMetricValues(DesktopDevice *desktopDevice);

    static QString metricDeviceId(Device *deviceObject);
    static QString metricDisplayName(Device *deviceObject, const QString &deviceId);
    static bool hasMetric(Device *deviceObject, Metrics::MetricId metricId);
    QVariant metricValue(Device *deviceObject, Metrics::MetricId metricId) const;

    bool m_metricsDiscovered = false;
    QList<MetricDescriptor> m_availableMetrics;
};

#endif // METRICSSERVICE_H
