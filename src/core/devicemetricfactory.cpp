#include "devicemetricfactory.h"

#include "storages/desktopdevice.h"

namespace {
struct DeviceMetricDefinition
{
    Metrics::MetricId metricId = Metrics::MetricId::Unknown;
    const char *legacyPropertyName = nullptr;
};

QString displayName(Device *deviceObject, const QString &fallbackDeviceId)
{
    const QString deviceName = deviceObject ? deviceObject->name() : QString();
    return deviceName.isEmpty() ? fallbackDeviceId.toUpper() : deviceName;
}

MetricDescriptor makeDescriptor(Device *deviceObject,
                                const QString &deviceId,
                                Metrics::MetricId metricId)
{
    return {
        deviceId,
        metricId,
        displayName(deviceObject, deviceId),
        Metrics::metricUnit(metricId)
    };
}

QVariant legacyPropertyValue(Device *deviceObject, const char *propertyName)
{
    if (!deviceObject || !propertyName)
        return {};

    return deviceObject->property(propertyName);
}

QList<MetricDescriptor> createDescriptors(Device *deviceObject,
                                          const QString &deviceId,
                                          const QList<DeviceMetricDefinition> &definitions)
{
    QList<MetricDescriptor> descriptors;
    if (!deviceObject || deviceId.isEmpty())
        return descriptors;

    for (const DeviceMetricDefinition &definition : definitions) {
        if (definition.metricId == Metrics::MetricId::Unknown)
            continue;

        descriptors.push_back(makeDescriptor(deviceObject, deviceId, definition.metricId));
    }

    return descriptors;
}

const QList<DeviceMetricDefinition> &processorMetricDefinitions()
{
    static const QList<DeviceMetricDefinition> definitions = {
        { Metrics::MetricId::Loading, "loading" },
        { Metrics::MetricId::Temperature, "temperature" },
        { Metrics::MetricId::Frequency, "frequency" }
    };

    return definitions;
}

const QList<DeviceMetricDefinition> &memoryMetricDefinitions()
{
    static const QList<DeviceMetricDefinition> definitions = {
        { Metrics::MetricId::Loading, "loading" },
        { Metrics::MetricId::Temperature, "temperature" },
        { Metrics::MetricId::Frequency, "frequency" }
    };

    return definitions;
}

const QList<DeviceMetricDefinition> &videoCardMetricDefinitions()
{
    static const QList<DeviceMetricDefinition> definitions = {
        { Metrics::MetricId::Loading, "loading" },
        { Metrics::MetricId::Temperature, "temperature" },
        { Metrics::MetricId::Frequency, "frequency" }
    };

    return definitions;
}

const QList<DeviceMetricDefinition> &hardDiskMetricDefinitions()
{
    static const QList<DeviceMetricDefinition> definitions = {
        { Metrics::MetricId::Loading, "load" }
    };

    return definitions;
}

const char *legacyPropertyName(const QList<DeviceMetricDefinition> &definitions,
                               Metrics::MetricId metricId)
{
    for (const DeviceMetricDefinition &definition : definitions) {
        if (definition.metricId == metricId)
            return definition.legacyPropertyName;
    }

    return nullptr;
}
}

QList<MetricDescriptor> DeviceMetricFactory::createDescriptors(DesktopDevice *desktopDevice)
{
    QList<MetricDescriptor> descriptors;
    if (!desktopDevice)
        return descriptors;

    const QList<Device *> devices = desktopDevice->devicesList();
    for (Device *deviceObject : devices)
        descriptors.append(createDescriptorsForDevice(deviceObject));

    return descriptors;
}

QString DeviceMetricFactory::deviceId(Device *deviceObject)
{
    if (!deviceObject)
        return {};

    switch (deviceObject->type()) {
    case Device::PROCESSOR:
        return QStringLiteral("cpu");
    case Device::MEMORY:
        return QStringLiteral("ram");
    case Device::VIDEO_CARD:
        return QStringLiteral("gpu");
    case Device::HARD_DISK:
        return QStringLiteral("hdd");
    default:
        return {};
    }
}

QVariant DeviceMetricFactory::metricValue(Device *deviceObject, Metrics::MetricId metricId)
{
    if (!deviceObject || metricId == Metrics::MetricId::Unknown)
        return {};

    switch (deviceObject->type()) {
    case Device::PROCESSOR:
        return processorMetricValue(deviceObject, metricId);
    case Device::MEMORY:
        return memoryMetricValue(deviceObject, metricId);
    case Device::VIDEO_CARD:
        return videoCardMetricValue(deviceObject, metricId);
    case Device::HARD_DISK:
        return hardDiskMetricValue(deviceObject, metricId);
    default:
        return {};
    }
}

QList<MetricDescriptor> DeviceMetricFactory::createDescriptorsForDevice(Device *deviceObject)
{
    if (!deviceObject)
        return {};

    switch (deviceObject->type()) {
    case Device::PROCESSOR:
        return parseProcessor(deviceObject);
    case Device::MEMORY:
        return parseMemory(deviceObject);
    case Device::VIDEO_CARD:
        return parseVideoCard(deviceObject);
    case Device::HARD_DISK:
        return parseHardDisk(deviceObject);
    default:
        return {};
    }
}

QList<MetricDescriptor> DeviceMetricFactory::parseProcessor(Device *deviceObject)
{
    // TODO(DeviceBuilder): replace Device* with the concrete processor class, e.g.
    // auto *processor = qobject_cast<ProcessorDevice *>(deviceObject);
    // and create descriptors only for metrics supported by processor.
    return createDescriptors(deviceObject, QStringLiteral("cpu"), processorMetricDefinitions());
}

QList<MetricDescriptor> DeviceMetricFactory::parseMemory(Device *deviceObject)
{
    // TODO(DeviceBuilder): cast to the concrete memory class and keep only
    // descriptors that are meaningful for that type.
    return createDescriptors(deviceObject, QStringLiteral("ram"), memoryMetricDefinitions());
}

QList<MetricDescriptor> DeviceMetricFactory::parseVideoCard(Device *deviceObject)
{
    // TODO(DeviceBuilder): cast to the concrete video-card class and fill
    // descriptors from its typed API.
    return createDescriptors(deviceObject, QStringLiteral("gpu"), videoCardMetricDefinitions());
}

QList<MetricDescriptor> DeviceMetricFactory::parseHardDisk(Device *deviceObject)
{
    // TODO(DeviceBuilder): cast to the concrete hard-disk class and fill
    // descriptors from its typed API.
    return createDescriptors(deviceObject, QStringLiteral("hdd"), hardDiskMetricDefinitions());
}

QVariant DeviceMetricFactory::processorMetricValue(Device *deviceObject, Metrics::MetricId metricId)
{
    // TODO(DeviceBuilder): replace the temporary legacy fallback with typed calls:
    // auto *processor = qobject_cast<ProcessorDevice *>(deviceObject);
    // switch (metricId) { case Metrics::MetricId::Loading: return processor->loading(); ... }
    return legacyPropertyValue(deviceObject, legacyPropertyName(processorMetricDefinitions(), metricId));
}

QVariant DeviceMetricFactory::memoryMetricValue(Device *deviceObject, Metrics::MetricId metricId)
{
    // TODO(DeviceBuilder): replace the temporary legacy fallback with typed calls
    // to the concrete memory device class.
    return legacyPropertyValue(deviceObject, legacyPropertyName(memoryMetricDefinitions(), metricId));
}

QVariant DeviceMetricFactory::videoCardMetricValue(Device *deviceObject, Metrics::MetricId metricId)
{
    // TODO(DeviceBuilder): replace the temporary legacy fallback with typed calls
    // to the concrete video-card device class.
    return legacyPropertyValue(deviceObject, legacyPropertyName(videoCardMetricDefinitions(), metricId));
}

QVariant DeviceMetricFactory::hardDiskMetricValue(Device *deviceObject, Metrics::MetricId metricId)
{
    // TODO(DeviceBuilder): replace the temporary legacy fallback with typed calls:
    // auto *hardDisk = qobject_cast<HardDiskDevice *>(deviceObject);
    // case Metrics::MetricId::Loading: return hardDisk->load();
    return legacyPropertyValue(deviceObject, legacyPropertyName(hardDiskMetricDefinitions(), metricId));
}
