#include "lhmsnapshotparser.h"

#include "parsedsensorvalue.h"

#include <QJsonArray>
#include <QJsonValue>

namespace {

HardwareKind hardwareKindFromId(const QString &hardwareId, const QString &)
{
    if (hardwareId.startsWith(QStringLiteral("/amdcpu")) || hardwareId.startsWith(QStringLiteral("/intelcpu")))
        return HardwareKind::Cpu;
    if (hardwareId.startsWith(QStringLiteral("/gpu-nvidia")) || hardwareId.startsWith(QStringLiteral("/gpu-amd")))
        return HardwareKind::Gpu;
    if (hardwareId.startsWith(QStringLiteral("/ram")) || hardwareId.startsWith(QStringLiteral("/vram")))
        return HardwareKind::Memory;
    if (hardwareId.startsWith(QStringLiteral("/memory/dimm")))
        return HardwareKind::MemoryModule;
    if (hardwareId.startsWith(QStringLiteral("/nvme"))
        || hardwareId.startsWith(QStringLiteral("/ssd"))
        || hardwareId.startsWith(QStringLiteral("/hdd"))) {
        return HardwareKind::Storage;
    }
    if (hardwareId.startsWith(QStringLiteral("/nic")))
        return HardwareKind::Network;
    if (hardwareId.startsWith(QStringLiteral("/battery")))
        return HardwareKind::Battery;
    if (hardwareId.startsWith(QStringLiteral("/motherboard")) || hardwareId.startsWith(QStringLiteral("/lpc")))
        return HardwareKind::Motherboard;

    return HardwareKind::Unknown;
}

MetricKind metricKindFromType(const QString &type)
{
    if (type == QStringLiteral("Voltage"))
        return MetricKind::Voltage;
    if (type == QStringLiteral("Current"))
        return MetricKind::Current;
    if (type == QStringLiteral("Power"))
        return MetricKind::Power;
    if (type == QStringLiteral("Clock"))
        return MetricKind::Clock;
    if (type == QStringLiteral("Temperature"))
        return MetricKind::Temperature;
    if (type == QStringLiteral("Load"))
        return MetricKind::Load;
    if (type == QStringLiteral("Fan"))
        return MetricKind::Fan;
    if (type == QStringLiteral("Control"))
        return MetricKind::Control;
    if (type == QStringLiteral("Data"))
        return MetricKind::Data;
    if (type == QStringLiteral("Throughput"))
        return MetricKind::Throughput;
    if (type == QStringLiteral("Level"))
        return MetricKind::Level;
    if (type == QStringLiteral("Factor"))
        return MetricKind::Factor;
    if (type == QStringLiteral("Timing"))
        return MetricKind::Timing;
    if (type == QStringLiteral("Energy"))
        return MetricKind::Energy;

    return MetricKind::Unknown;
}

ParsedSensorValue parseValueField(const QJsonObject &node, const QString &rawKey, const QString &displayKey)
{
    if (node.contains(rawKey))
        return parseSensorValue(node.value(rawKey).toString());

    return parseSensorValue(node.value(displayKey).toString());
}

void assignMeasurementUnit(Measurement &measurement,
                           const ParsedSensorValue &value,
                           const ParsedSensorValue &min,
                           const ParsedSensorValue &max)
{
    if (value.unit != Unit::Unknown) {
        measurement.unit = value.unit;
    } else if (min.unit != Unit::Unknown) {
        measurement.unit = min.unit;
    } else {
        measurement.unit = max.unit;
    }
}

void parseNode(const QJsonObject &node, const QString &parentHardwareId, HardwareSnapshot &snapshot)
{
    QString currentHardwareId = parentHardwareId;

    if (node.contains(QStringLiteral("HardwareId"))) {
        HardwareDevice device;
        device.id = node.value(QStringLiteral("HardwareId")).toString();
        device.name = node.value(QStringLiteral("Text")).toString();
        device.kind = hardwareKindFromId(device.id, node.value(QStringLiteral("ImageURL")).toString());

        snapshot.devices.append(device);
        currentHardwareId = device.id;
    }

    if (node.contains(QStringLiteral("SensorId"))) {
        const ParsedSensorValue value = parseValueField(node, QStringLiteral("RawValue"), QStringLiteral("Value"));
        const ParsedSensorValue min = parseValueField(node, QStringLiteral("RawMin"), QStringLiteral("Min"));
        const ParsedSensorValue max = parseValueField(node, QStringLiteral("RawMax"), QStringLiteral("Max"));

        Measurement measurement;
        measurement.id = node.value(QStringLiteral("SensorId")).toString();
        measurement.deviceId = currentHardwareId;
        measurement.name = node.value(QStringLiteral("Text")).toString();
        measurement.kind = metricKindFromType(node.value(QStringLiteral("Type")).toString());
        measurement.value = value.value;
        measurement.min = min.value;
        measurement.max = max.value;
        assignMeasurementUnit(measurement, value, min, max);

        snapshot.measurements.append(measurement);
    }

    const QJsonArray children = node.value(QStringLiteral("Children")).toArray();
    for (const QJsonValue &child : children) {
        if (child.isObject())
            parseNode(child.toObject(), currentHardwareId, snapshot);
    }
}

} // namespace

HardwareSnapshot LhmSnapshotParser::parse(const QJsonObject &root) const
{
    HardwareSnapshot snapshot;
    parseNode(root, {}, snapshot);
    return snapshot;
}
