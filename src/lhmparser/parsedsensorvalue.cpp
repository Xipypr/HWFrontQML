#include "parsedsensorvalue.h"

#include <QLocale>

namespace {

Unit parseUnit(const QString &rawUnit)
{
    const QString unit = rawUnit.trimmed();
    if (unit.isEmpty())
        return Unit::None;
    if (unit == QStringLiteral("%"))
        return Unit::Percent;
    if (unit == QStringLiteral("°C"))
        return Unit::Celsius;
    if (unit == QStringLiteral("V"))
        return Unit::Volt;
    if (unit == QStringLiteral("W"))
        return Unit::Watt;
    if (unit == QStringLiteral("A"))
        return Unit::Ampere;
    if (unit == QStringLiteral("RPM"))
        return Unit::Rpm;
    if (unit == QStringLiteral("MHz"))
        return Unit::Megahertz;
    if (unit == QStringLiteral("MB"))
        return Unit::Megabyte;
    if (unit == QStringLiteral("GB"))
        return Unit::Gigabyte;
    if (unit == QStringLiteral("KB/s"))
        return Unit::KilobytePerSecond;
    if (unit == QStringLiteral("MB/s"))
        return Unit::MegabytePerSecond;
    if (unit == QStringLiteral("ns"))
        return Unit::Nanosecond;
    if (unit == QStringLiteral("mWh"))
        return Unit::MilliwattHour;

    return Unit::Unknown;
}

bool isNumberCharacter(QChar character)
{
    return character.isDigit()
        || character == QLatin1Char(',')
        || character == QLatin1Char('.')
        || character == QLatin1Char('-')
        || character == QLatin1Char('+');
}

} // namespace

ParsedSensorValue parseSensorValue(const QString &raw)
{
    const QString trimmed = raw.trimmed();
    if (trimmed.isEmpty() || trimmed == QStringLiteral("-"))
        return {};

    int numberLength = 0;
    while (numberLength < trimmed.size() && isNumberCharacter(trimmed.at(numberLength)))
        ++numberLength;

    if (numberLength == 0)
        return {};

    const QString numberText = QString(trimmed.left(numberLength)).replace(QLatin1Char(','), QLatin1Char('.'));

    bool ok = false;
    const float value = QLocale::c().toFloat(numberText, &ok);
    if (!ok)
        return {};

    ParsedSensorValue parsed;
    parsed.value = value;
    parsed.unit = parseUnit(trimmed.mid(numberLength));

    return parsed;
}
