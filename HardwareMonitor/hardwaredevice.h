#ifndef HARDWAREDEVICE_H
#define HARDWAREDEVICE_H

#include "hardwarekind.h"

#include <QString>

struct HardwareDevice
{
    QString id;
    QString name;
    HardwareKind kind = HardwareKind::Unknown;
};

#endif // HARDWAREDEVICE_H
