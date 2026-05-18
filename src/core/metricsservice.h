#ifndef METRICSSERVICE_H
#define METRICSSERVICE_H

#include "metricdescriptor.h"

#include <QObject>
#include <QString>
#include <QVariant>

class DesktopDevice;

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

    bool m_metricsDiscovered = false;
    QList<MetricDescriptor> m_availableMetrics;
};

#endif // METRICSSERVICE_H
