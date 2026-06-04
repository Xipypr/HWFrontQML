#ifndef LHMCLASSIFICATION_H
#define LHMCLASSIFICATION_H

#include "hardwarekind.h"
#include "metrickind.h"

#include <QString>

HardwareKind hardwareKindFromLhmHardwareId(const QString &hardwareId);
MetricKind metricKindFromLhmType(const QString &type);

#endif // LHMCLASSIFICATION_H
