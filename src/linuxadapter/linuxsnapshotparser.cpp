#include "linuxsnapshotparser.h"

#include <QHash>
#include <QJsonArray>
#include <QJsonValue>

namespace {

constexpr auto CpuTotalLoadSuffix = "/load/total";
constexpr auto CpuDashboardLoadSuffix = "/load/0";
constexpr auto CpuPackageTemperatureSuffix = "/temperature/package";
constexpr auto CpuDashboardTemperatureSuffix = "/temperature/2";
constexpr auto CpuAverageClockSuffix = "/clock/average";
constexpr auto CpuDashboardClockSuffix = "/clock/1";
constexpr auto GpuCoreLoadSuffix = "/load/core";
constexpr auto GpuDashboardLoadSuffix = "/load/0";
constexpr auto GpuCoreTemperatureSuffix = "/temperature/core";
constexpr auto GpuDashboardTemperatureSuffix = "/temperature/0";
constexpr auto UsedLoadSuffix = "/load/used";
constexpr auto MemoryDashboardLoadSuffix = "/load/0";
constexpr auto StorageDashboardLoadSuffix = "/load/30";

HardwareKind hardwareKindFromLinuxKind(const QString &kind)
{
    const QString normalized = kind.trimmed().toLower();
    if (normalized == QStringLiteral("cpu"))
        return HardwareKind::Cpu;
    if (normalized == QStringLiteral("gpu"))
        return HardwareKind::Gpu;
    if (normalized == QStringLiteral("memory"))
        return HardwareKind::Memory;
    if (normalized == QStringLiteral("storage"))
        return HardwareKind::Storage;
    if (normalized == QStringLiteral("network"))
        return HardwareKind::Network;

    return HardwareKind::Unknown;
}

MetricKind metricKindFromLinuxKind(const QString &kind)
{
    const QString normalized = kind.trimmed().toLower();
    if (normalized == QStringLiteral("load"))
        return MetricKind::Load;
    if (normalized == QStringLiteral("temperature"))
        return MetricKind::Temperature;
    if (normalized == QStringLiteral("clock"))
        return MetricKind::Clock;
    if (normalized == QStringLiteral("level"))
        return MetricKind::Level;
    if (normalized == QStringLiteral("power"))
        return MetricKind::Power;
    if (normalized == QStringLiteral("voltage"))
        return MetricKind::Voltage;
    if (normalized == QStringLiteral("current"))
        return MetricKind::Current;
    if (normalized == QStringLiteral("fan"))
        return MetricKind::Fan;
    if (normalized == QStringLiteral("data"))
        return MetricKind::Data;
    if (normalized == QStringLiteral("throughput"))
        return MetricKind::Throughput;

    return MetricKind::Unknown;
}

Unit unitFromLinuxUnit(const QString &unit)
{
    const QString normalized = unit.trimmed().toLower();
    if (normalized == QStringLiteral("none"))
        return Unit::None;
    if (normalized == QStringLiteral("percent"))
        return Unit::Percent;
    if (normalized == QStringLiteral("celsius"))
        return Unit::Celsius;
    if (normalized == QStringLiteral("volt"))
        return Unit::Volt;
    if (normalized == QStringLiteral("watt"))
        return Unit::Watt;
    if (normalized == QStringLiteral("ampere"))
        return Unit::Ampere;
    if (normalized == QStringLiteral("rpm"))
        return Unit::Rpm;
    if (normalized == QStringLiteral("megahertz"))
        return Unit::Megahertz;
    if (normalized == QStringLiteral("megabyte"))
        return Unit::Megabyte;
    if (normalized == QStringLiteral("gigabyte"))
        return Unit::Gigabyte;
    if (normalized == QStringLiteral("kilobyte_per_second"))
        return Unit::KilobytePerSecond;
    if (normalized == QStringLiteral("megabyte_per_second"))
        return Unit::MegabytePerSecond;
    if (normalized == QStringLiteral("nanosecond"))
        return Unit::Nanosecond;
    if (normalized == QStringLiteral("milliwatt_hour"))
        return Unit::MilliwattHour;

    return Unit::Unknown;
}

std::optional<float> optionalFloatFromJson(const QJsonValue &value)
{
    if (value.isNull() || value.isUndefined())
        return std::nullopt;

    if (!value.isDouble())
        return std::nullopt;

    return static_cast<float>(value.toDouble());
}

QString normalizedMeasurementId(const Measurement &measurement, HardwareKind deviceKind)
{
    if (deviceKind == HardwareKind::Cpu && measurement.kind == MetricKind::Load
            && measurement.id.endsWith(QString::fromLatin1(CpuTotalLoadSuffix))) {
        return measurement.deviceId + QString::fromLatin1(CpuDashboardLoadSuffix);
    }

    if (deviceKind == HardwareKind::Cpu && measurement.kind == MetricKind::Temperature
            && measurement.id.endsWith(QString::fromLatin1(CpuPackageTemperatureSuffix))) {
        return measurement.deviceId + QString::fromLatin1(CpuDashboardTemperatureSuffix);
    }

    if (deviceKind == HardwareKind::Cpu && measurement.kind == MetricKind::Clock
            && measurement.id.endsWith(QString::fromLatin1(CpuAverageClockSuffix))) {
        return measurement.deviceId + QString::fromLatin1(CpuDashboardClockSuffix);
    }

    if (deviceKind == HardwareKind::Gpu && measurement.kind == MetricKind::Load
            && measurement.id.endsWith(QString::fromLatin1(GpuCoreLoadSuffix))) {
        return measurement.deviceId + QString::fromLatin1(GpuDashboardLoadSuffix);
    }

    if (deviceKind == HardwareKind::Gpu && measurement.kind == MetricKind::Temperature
            && measurement.id.endsWith(QString::fromLatin1(GpuCoreTemperatureSuffix))) {
        return measurement.deviceId + QString::fromLatin1(GpuDashboardTemperatureSuffix);
    }

    if (deviceKind == HardwareKind::Memory && measurement.kind == MetricKind::Load
            && measurement.id.endsWith(QString::fromLatin1(UsedLoadSuffix))) {
        return measurement.deviceId + QString::fromLatin1(MemoryDashboardLoadSuffix);
    }

    if (deviceKind == HardwareKind::Storage && measurement.kind == MetricKind::Load
            && measurement.id.endsWith(QString::fromLatin1(UsedLoadSuffix))) {
        return measurement.deviceId + QString::fromLatin1(StorageDashboardLoadSuffix);
    }

    return measurement.id;
}

} // namespace

HardwareSnapshot LinuxSnapshotParser::parse(const QJsonObject &root) const
{
    HardwareSnapshot snapshot;
    QHash<QString, HardwareKind> deviceKindsById;

    const QJsonArray devices = root.value(QStringLiteral("devices")).toArray();
    for (const QJsonValue &value : devices) {
        if (!value.isObject())
            continue;

        const QJsonObject object = value.toObject();
        HardwareDevice device;
        device.id = object.value(QStringLiteral("id")).toString().trimmed();
        device.name = object.value(QStringLiteral("name")).toString().trimmed();
        device.kind = hardwareKindFromLinuxKind(object.value(QStringLiteral("kind")).toString());

        if (!device.id.isEmpty()) {
            snapshot.devices.append(device);
            deviceKindsById.insert(device.id, device.kind);
        }
    }

    const QJsonArray measurements = root.value(QStringLiteral("measurements")).toArray();
    for (const QJsonValue &value : measurements) {
        if (!value.isObject())
            continue;

        const QJsonObject object = value.toObject();
        Measurement measurement;
        measurement.id = object.value(QStringLiteral("id")).toString().trimmed();
        measurement.deviceId = object.value(QStringLiteral("deviceId")).toString().trimmed();
        measurement.name = object.value(QStringLiteral("name")).toString().trimmed();
        measurement.kind = metricKindFromLinuxKind(object.value(QStringLiteral("kind")).toString());
        measurement.unit = unitFromLinuxUnit(object.value(QStringLiteral("unit")).toString());
        measurement.value = optionalFloatFromJson(object.value(QStringLiteral("value")));
        measurement.min = optionalFloatFromJson(object.value(QStringLiteral("min")));
        measurement.max = optionalFloatFromJson(object.value(QStringLiteral("max")));
        measurement.id = normalizedMeasurementId(measurement, deviceKindsById.value(measurement.deviceId, HardwareKind::Unknown));

        if (!measurement.id.isEmpty() && !measurement.deviceId.isEmpty())
            snapshot.measurements.append(measurement);
    }

    return snapshot;
}

QString LinuxSnapshotParser::displayName(const QJsonObject &root) const
{
    const QJsonObject host = root.value(QStringLiteral("host")).toObject();
    const QString name = host.value(QStringLiteral("name")).toString().trimmed();
    if (!name.isEmpty())
        return name;

    return host.value(QStringLiteral("id")).toString().trimmed();
}
