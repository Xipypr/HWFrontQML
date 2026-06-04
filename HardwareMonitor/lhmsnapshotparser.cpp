#include "lhmsnapshotparser.h"

#include "lhmclassification.h"
#include "parsedsensorvalue.h"

#include <QJsonArray>
#include <QJsonValue>

namespace {

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
        device.kind = hardwareKindFromLhmHardwareId(device.id);

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
        measurement.kind = metricKindFromLhmType(node.value(QStringLiteral("Type")).toString());
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
