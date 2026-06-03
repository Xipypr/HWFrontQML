#ifndef HARDWARESNAPSHOT_H
#define HARDWARESNAPSHOT_H

#include "hardwaredevice.h"
#include "measurement.h"

#include <QList>

struct HardwareSnapshot
{
    QList<HardwareDevice> devices;
    QList<Measurement> measurements;
};

#endif // HARDWARESNAPSHOT_H
