#ifndef LEGACYDESKTOPDEVICEADAPTER_H
#define LEGACYDESKTOPDEVICEADAPTER_H

#include "hardwaresnapshot.h"

class DesktopDevice;

class LegacyDesktopDeviceAdapter
{
public:
    void apply(const HardwareSnapshot &snapshot, DesktopDevice *desktop) const;
};

#endif // LEGACYDESKTOPDEVICEADAPTER_H
