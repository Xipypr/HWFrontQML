#ifndef METRICDESCRIPTOR_H
#define METRICDESCRIPTOR_H

#include <QList>
#include <QMetaType>
#include <QObject>
#include <QString>

#include <hardwarekind.h>

namespace Metrics {
Q_NAMESPACE

enum class MetricId {
    Unknown = 0,
    Loading,
    Temperature,
    Frequency,
    BatteryLevel,
    NetworkUpload,
    NetworkDownload
};
Q_ENUM_NS(MetricId)

inline QString metricIdToString(MetricId metricId)
{
    switch (metricId) {
    case MetricId::Loading:
        return QStringLiteral("loading");
    case MetricId::Temperature:
        return QStringLiteral("temperature");
    case MetricId::Frequency:
        return QStringLiteral("frequency");
    case MetricId::BatteryLevel:
        return QStringLiteral("batteryLevel");
    case MetricId::NetworkUpload:
        return QStringLiteral("networkUpload");
    case MetricId::NetworkDownload:
        return QStringLiteral("networkDownload");
    case MetricId::Unknown:
        return QStringLiteral("unknown");
    }

    return QStringLiteral("unknown");
}

inline MetricId metricIdFromString(const QString &metricId)
{
    const QString normalizedMetricId = metricId.toLower();
    if (normalizedMetricId == QStringLiteral("loading"))
        return MetricId::Loading;
    if (normalizedMetricId == QStringLiteral("temperature"))
        return MetricId::Temperature;
    if (normalizedMetricId == QStringLiteral("frequency"))
        return MetricId::Frequency;
    if (normalizedMetricId == QStringLiteral("batterylevel"))
        return MetricId::BatteryLevel;
    if (normalizedMetricId == QStringLiteral("networkupload"))
        return MetricId::NetworkUpload;
    if (normalizedMetricId == QStringLiteral("networkdownload"))
        return MetricId::NetworkDownload;

    return MetricId::Unknown;
}

inline QString metricUnit(MetricId metricId)
{
    switch (metricId) {
    case MetricId::Loading:
        return QStringLiteral("%");
    case MetricId::Temperature:
        return QStringLiteral("°C");
    case MetricId::Frequency:
        return QStringLiteral("MHz");
    case MetricId::BatteryLevel:
        return QStringLiteral("%");
    case MetricId::NetworkUpload:
    case MetricId::NetworkDownload:
        return QStringLiteral(" MB/s");
    case MetricId::Unknown:
        return {};
    }

    return {};
}
}

struct MetricDescriptor
{
    MetricDescriptor() = delete;

    QString deviceId;
    HardwareKind hardwareKind = HardwareKind::Unknown;
    Metrics::MetricId metricId;
    QString displayName;
    QString unit;
    bool showProgressBar = false;
};

Q_DECLARE_METATYPE(Metrics::MetricId)
Q_DECLARE_METATYPE(MetricDescriptor)
Q_DECLARE_METATYPE(QList<MetricDescriptor>)

#endif // METRICDESCRIPTOR_H
