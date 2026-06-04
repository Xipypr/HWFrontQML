#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include "metrickind.h"
#include "unit.h"

#include <QString>

#include <optional>

struct Measurement
{
    QString id;
    QString deviceId;
    QString name;
    MetricKind kind = MetricKind::Unknown;
    Unit unit = Unit::Unknown;

    std::optional<float> value;
    std::optional<float> min;
    std::optional<float> max;
};

#endif // MEASUREMENT_H
