#ifndef METRICDESCRIPTOR_H
#define METRICDESCRIPTOR_H

#include <QList>
#include <QMetaType>
#include <QObject>
#include <QString>

namespace Metrics {
Q_NAMESPACE

enum class MetricId {
    Unknown = 0,
    Loading,
    Temperature,
    Frequency
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
    case MetricId::Unknown:
        return {};
    }

    return {};
}
}

struct MetricDescriptor
{
    MetricDescriptor() = delete;

    MetricDescriptor(const QString &deviceId,
                     Metrics::MetricId metricId,
                     const QString &displayName)
        : MetricDescriptor(deviceId, metricId, displayName, Metrics::metricUnit(metricId))
    {
    }

    MetricDescriptor(const QString &deviceId,
                     Metrics::MetricId metricId,
                     const QString &displayName,
                     const QString &unit)
        : deviceId(deviceId)
        , metricId(metricId)
        , displayName(displayName)
        , unit(unit)
    {
    }

    static MetricDescriptor createLoadingDescr(const QString &deviceId, const QString &displayName)
    {
        return { deviceId, Metrics::MetricId::Loading, displayName };
    }

    static MetricDescriptor createTempDescr(const QString &deviceId, const QString &displayName)
    {
        return createTemperatureDescr(deviceId, displayName);
    }

    static MetricDescriptor createTemperatureDescr(const QString &deviceId, const QString &displayName)
    {
        return { deviceId, Metrics::MetricId::Temperature, displayName };
    }

    static MetricDescriptor createFrequencyDescr(const QString &deviceId, const QString &displayName)
    {
        return { deviceId, Metrics::MetricId::Frequency, displayName };
    }

    QString deviceId;
    Metrics::MetricId metricId;
    QString displayName;
    QString unit;
};

Q_DECLARE_METATYPE(Metrics::MetricId)
Q_DECLARE_METATYPE(MetricDescriptor)
Q_DECLARE_METATYPE(QList<MetricDescriptor>)

#endif // METRICDESCRIPTOR_H
