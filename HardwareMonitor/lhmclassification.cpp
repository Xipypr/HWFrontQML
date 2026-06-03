#include "lhmclassification.h"

HardwareKind hardwareKindFromLhmHardwareId(const QString &hardwareId)
{
    if (hardwareId.startsWith(QStringLiteral("/amdcpu")) || hardwareId.startsWith(QStringLiteral("/intelcpu")))
        return HardwareKind::Cpu;
    if (hardwareId.startsWith(QStringLiteral("/gpu-nvidia")) || hardwareId.startsWith(QStringLiteral("/gpu-amd")))
        return HardwareKind::Gpu;
    if (hardwareId.startsWith(QStringLiteral("/memory/dimm")))
        return HardwareKind::MemoryModule;
    if (hardwareId.startsWith(QStringLiteral("/ram")) || hardwareId.startsWith(QStringLiteral("/vram")))
        return HardwareKind::Memory;
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

MetricKind metricKindFromLhmType(const QString &type)
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
