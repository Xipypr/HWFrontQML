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
    Frequency,
    BatteryLevel,
    FanSpeed
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
    case MetricId::FanSpeed:
        return QStringLiteral("fanSpeed");
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
    if (normalizedMetricId == QStringLiteral("fanspeed"))
        return MetricId::FanSpeed;

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
    case MetricId::FanSpeed:
        return QStringLiteral("RPM");
    case MetricId::Unknown:
        return {};
    }

    return {};
}
}

struct MetricDescriptor
{
    MetricDescriptor() = delete;

    static MetricDescriptor createLoadingDescr(const QString &deviceId, const QString &displayName)
    {
        return { deviceId, Metrics::MetricId::Loading, displayName, metricUnit(Metrics::MetricId::Loading), true };
    }

    static MetricDescriptor createTemperatureDescr(const QString &deviceId, const QString &displayName)
    {
        return { deviceId, Metrics::MetricId::Temperature, displayName, metricUnit(Metrics::MetricId::Temperature), false };
    }

    static MetricDescriptor createFrequencyDescr(const QString &deviceId, const QString &displayName)
    {
        return { deviceId, Metrics::MetricId::Frequency, displayName, metricUnit(Metrics::MetricId::Frequency), false };
    }

    static MetricDescriptor createBatteryLevelDescr(const QString &deviceId, const QString &displayName)
    {
        return { deviceId, Metrics::MetricId::BatteryLevel, displayName, metricUnit(Metrics::MetricId::BatteryLevel), true };
    }

    static MetricDescriptor createFanSpeedDescr(const QString &deviceId, const QString &displayName)
    {
        return { deviceId, Metrics::MetricId::FanSpeed, displayName, metricUnit(Metrics::MetricId::FanSpeed), false };
    }

    QString deviceId;
    Metrics::MetricId metricId;
    QString displayName;
    QString unit;
    bool showProgressBar = false;
};

Q_DECLARE_METATYPE(Metrics::MetricId)
Q_DECLARE_METATYPE(MetricDescriptor)
Q_DECLARE_METATYPE(QList<MetricDescriptor>)

#endif // METRICDESCRIPTOR_H
