#include "legacydesktopdeviceadapter.h"

#include <storages/cpu.h>
#include <storages/desktopdevice.h>
#include <storages/hdd.h>
#include <storages/ram.h>
#include <storages/videocard.h>

#include <QStringList>

#include <optional>

namespace {

QList<Measurement> measurementsForDevice(const HardwareSnapshot &snapshot,
                                         const QString &deviceId,
                                         MetricKind kind)
{
    QList<Measurement> measurements;
    for (const Measurement &measurement : snapshot.measurements) {
        if (measurement.deviceId == deviceId && measurement.kind == kind && measurement.value.has_value())
            measurements.append(measurement);
    }
    return measurements;
}

bool nameContainsAny(const QString &name, const QStringList &parts)
{
    for (const QString &part : parts) {
        if (name.contains(part, Qt::CaseInsensitive))
            return true;
    }
    return false;
}

std::optional<Measurement> findPreferredMeasurement(const QList<Measurement> &measurements,
                                                    const QStringList &preferredNames)
{
    for (const QString &preferredName : preferredNames) {
        for (const Measurement &measurement : measurements) {
            if (measurement.name.compare(preferredName, Qt::CaseInsensitive) == 0)
                return measurement;
        }
    }

    for (const Measurement &measurement : measurements) {
        if (nameContainsAny(measurement.name, preferredNames))
            return measurement;
    }

    if (!measurements.isEmpty())
        return measurements.first();

    return std::nullopt;
}

MinMaxValue toMinMaxValue(const Measurement &measurement)
{
    MinMaxValue value;
    value.name = measurement.name;
    value.min = measurement.min.value_or(0.0f);
    value.max = measurement.max.value_or(0.0f);
    value.value = measurement.value.value_or(0.0f);
    return value;
}

QList<MinMaxValue> singleMetricList(const Measurement &measurement)
{
    QList<MinMaxValue> values;
    values.append(toMinMaxValue(measurement));
    return values;
}

void adaptCpu(const HardwareSnapshot &snapshot, const HardwareDevice &device, DesktopDevice *desktop)
{
    Cpu *cpu = new Cpu;
    cpu->setName(device.name);

    const auto load = findPreferredMeasurement(
        measurementsForDevice(snapshot, device.id, MetricKind::Load),
        { QStringLiteral("CPU Total"), QStringLiteral("Total") });
    if (load.has_value())
        cpu->addLoadArr(singleMetricList(load.value()));

    const auto temperature = findPreferredMeasurement(
        measurementsForDevice(snapshot, device.id, MetricKind::Temperature),
        { QStringLiteral("Core (Tctl/Tdie)"), QStringLiteral("CPU Package"), QStringLiteral("Package"), QStringLiteral("Core Average") });
    if (temperature.has_value())
        cpu->addTemperaturesArr(singleMetricList(temperature.value()));

    desktop->addDevice(cpu);
}

void adaptGpu(const HardwareSnapshot &snapshot, const HardwareDevice &device, DesktopDevice *desktop)
{
    VideoCard *gpu = new VideoCard;
    gpu->setName(device.name);

    const auto load = findPreferredMeasurement(
        measurementsForDevice(snapshot, device.id, MetricKind::Load),
        { QStringLiteral("GPU Core"), QStringLiteral("D3D 3D"), QStringLiteral("Total") });
    if (load.has_value())
        gpu->addLoadArr(singleMetricList(load.value()));

    const auto temperature = findPreferredMeasurement(
        measurementsForDevice(snapshot, device.id, MetricKind::Temperature),
        { QStringLiteral("GPU Core"), QStringLiteral("Temperature") });
    if (temperature.has_value())
        gpu->addTemperaturesArr(singleMetricList(temperature.value()));

    desktop->addDevice(gpu);
}

void adaptMemory(const HardwareSnapshot &snapshot, const HardwareDevice &device, DesktopDevice *desktop)
{
    Ram *ram = new Ram;
    ram->setName(device.name);

    const auto load = findPreferredMeasurement(
        measurementsForDevice(snapshot, device.id, MetricKind::Load),
        { QStringLiteral("Memory"), QStringLiteral("Memory Used") });
    if (load.has_value())
        ram->setLoad(toMinMaxValue(load.value()));

    const QList<Measurement> dataMeasurements = measurementsForDevice(snapshot, device.id, MetricKind::Data);

    const auto used = findPreferredMeasurement(
        dataMeasurements,
        { QStringLiteral("Used Memory"), QStringLiteral("Memory Used"), QStringLiteral("Used") });
    if (used.has_value())
        ram->setUsedMemory(toMinMaxValue(used.value()));

    const auto available = findPreferredMeasurement(
        dataMeasurements,
        { QStringLiteral("Available Memory"), QStringLiteral("Memory Available"), QStringLiteral("Available") });
    if (available.has_value())
        ram->setAvailableMemory(toMinMaxValue(available.value()));

    desktop->addDevice(ram);
}

void adaptStorage(const HardwareSnapshot &snapshot, const HardwareDevice &device, DesktopDevice *desktop)
{
    Hdd *storage = new Hdd;
    storage->setName(device.name);

    const auto load = findPreferredMeasurement(
        measurementsForDevice(snapshot, device.id, MetricKind::Load),
        { QStringLiteral("Used Space"), QStringLiteral("Total Activity") });
    if (load.has_value())
        storage->setLoad(toMinMaxValue(load.value()));

    const auto temperature = findPreferredMeasurement(
        measurementsForDevice(snapshot, device.id, MetricKind::Temperature),
        { QStringLiteral("Temperature") });
    if (temperature.has_value())
        storage->setTemperature(toMinMaxValue(temperature.value()));

    desktop->addDevice(storage);
}

} // namespace

void LegacyDesktopDeviceAdapter::apply(const HardwareSnapshot &snapshot, DesktopDevice *desktop) const
{
    if (desktop == nullptr)
        return;

    desktop->clearDevices();

    for (const HardwareDevice &device : snapshot.devices) {
        switch (device.kind) {
        case HardwareKind::Cpu:
            adaptCpu(snapshot, device, desktop);
            break;
        case HardwareKind::Gpu:
            adaptGpu(snapshot, device, desktop);
            break;
        case HardwareKind::Memory:
            adaptMemory(snapshot, device, desktop);
            break;
        case HardwareKind::Storage:
            adaptStorage(snapshot, device, desktop);
            break;
        case HardwareKind::Unknown:
        case HardwareKind::Computer:
        case HardwareKind::Motherboard:
        case HardwareKind::MemoryModule:
        case HardwareKind::Network:
        case HardwareKind::Battery:
            break;
        }
    }
}
