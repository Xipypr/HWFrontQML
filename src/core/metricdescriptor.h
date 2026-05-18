#ifndef METRICDESCRIPTOR_H
#define METRICDESCRIPTOR_H

#include "storages/desktopdevice.h"

#include <QList>
#include <QMetaType>
#include <QObject>
#include <QString>

#include <utility>

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

using DeviceMetricType = decltype(std::declval<Device>().type());

inline bool isValidDeviceMetricType(DeviceMetricType deviceType)
{
    switch (deviceType) {
    case Device::PROCESSOR:
    case Device::MEMORY:
    case Device::VIDEO_CARD:
    case Device::HARD_DISK:
        return true;
    default:
        return false;
    }
}

struct MetricDescriptor
{
    MetricDescriptor() = delete;

    MetricDescriptor(DeviceMetricType deviceType,
                     Metrics::MetricId metricId,
                     const QString &displayName)
        : MetricDescriptor(deviceType, metricId, displayName, Metrics::metricUnit(metricId))
    {
    }

    MetricDescriptor(DeviceMetricType deviceType,
                     Metrics::MetricId metricId,
                     const QString &displayName,
                     const QString &unit)
        : deviceType(deviceType)
        , metricId(metricId)
        , displayName(displayName)
        , unit(unit)
    {
    }

    static MetricDescriptor createLoadingDescr(DeviceMetricType deviceType, const QString &displayName)
    {
        return { deviceType, Metrics::MetricId::Loading, displayName };
    }

    static MetricDescriptor createTempDescr(DeviceMetricType deviceType, const QString &displayName)
    {
        return createTemperatureDescr(deviceType, displayName);
    }

    static MetricDescriptor createTemperatureDescr(DeviceMetricType deviceType, const QString &displayName)
    {
        return { deviceType, Metrics::MetricId::Temperature, displayName };
    }

    static MetricDescriptor createFrequencyDescr(DeviceMetricType deviceType, const QString &displayName)
    {
        return { deviceType, Metrics::MetricId::Frequency, displayName };
    }

    DeviceMetricType deviceType;
    Metrics::MetricId metricId;
    QString displayName;
    QString unit;
};

Q_DECLARE_METATYPE(Metrics::MetricId)
Q_DECLARE_METATYPE(MetricDescriptor)
Q_DECLARE_METATYPE(QList<MetricDescriptor>)

#endif // METRICDESCRIPTOR_H
