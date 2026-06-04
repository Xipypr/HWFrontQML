#ifndef PARSEDSENSORVALUE_H
#define PARSEDSENSORVALUE_H

#include "unit.h"

#include <QString>

#include <optional>

struct ParsedSensorValue
{
    std::optional<float> value;
    Unit unit = Unit::Unknown;
};

ParsedSensorValue parseSensorValue(const QString &raw);

#endif // PARSEDSENSORVALUE_H
