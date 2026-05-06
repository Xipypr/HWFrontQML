#ifndef METRICSSERVICE_H
#define METRICSSERVICE_H

#include <QList>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QVariantList>

class DesktopDevice;
class Device;

struct MetricDescriptor
{
    QString deviceId;
    QString metricId;
    QString displayName;
    QString unit;
};

Q_DECLARE_METATYPE(MetricDescriptor)
Q_DECLARE_METATYPE(QList<MetricDescriptor>)

class MetricsService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList availableMetrics READ availableMetrics NOTIFY availableMetricsChanged)

public:
    explicit MetricsService(QObject *parent = nullptr);

    QVariantList availableMetrics() const;
    QList<MetricDescriptor> metricDescriptors() const;

public slots:
    void processDeviceSnapshot(DesktopDevice *desktopDevice);

signals:
    void availableMetricsChanged(const QVariantList &metrics);
    void metricUpdated(const QString &deviceId,
                       const QString &metricId,
                       const QVariant &value);

private:
    void discoverMetrics(DesktopDevice *desktopDevice);
    void refreshMetricValues(DesktopDevice *desktopDevice);

    static QString metricDeviceId(Device *deviceObject);
    static QString metricDisplayName(Device *deviceObject, const QString &deviceId);
    static bool hasMetric(Device *deviceObject, const QString &metricId);
    static QVariantMap descriptorToVariantMap(const MetricDescriptor &descriptor);
    QVariant metricValue(Device *deviceObject, const QString &metricId) const;

    bool m_metricsDiscovered = false;
    QList<MetricDescriptor> m_availableMetrics;
};

#endif // METRICSSERVICE_H
