#include "devicemetricfactory.h"

#include "storages/desktopdevice.h"

namespace {
QString deviceTypeName(DeviceMetricType deviceType)
{
    switch (deviceType) {
    case Device::PROCESSOR:
        return QStringLiteral("CPU");
    case Device::MEMORY:
        return QStringLiteral("RAM");
    case Device::VIDEO_CARD:
        return QStringLiteral("GPU");
    case Device::HARD_DISK:
        return QStringLiteral("HDD");
    default:
        return {};
    }
}

QString displayName(Device *deviceObject, DeviceMetricType deviceType)
{
    const QString deviceName = deviceObject ? deviceObject->name() : QString();
    return deviceName.isEmpty() ? deviceTypeName(deviceType) : deviceName;
}

QList<MetricDescriptor> createCoreMetricDescriptors(Device *deviceObject, DeviceMetricType deviceType)
{
    const QString name = displayName(deviceObject, deviceType);
    return {
        MetricDescriptor::createLoadingDescr(deviceType, name),
        MetricDescriptor::createTempDescr(deviceType, name),
        MetricDescriptor::createFrequencyDescr(deviceType, name)
    };
}

QList<MetricDescriptor> createLoadingMetricDescriptor(Device *deviceObject, DeviceMetricType deviceType)
{
    return {
        MetricDescriptor::createLoadingDescr(deviceType, displayName(deviceObject, deviceType))
    };
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

DeviceMetricType DeviceMetricFactory::deviceType(Device *deviceObject)
{
    return deviceObject ? deviceObject->type() : DeviceMetricType{};
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
    return createCoreMetricDescriptors(deviceObject, Device::PROCESSOR);
}

QList<MetricDescriptor> DeviceMetricFactory::parseMemory(Device *deviceObject)
{
    return createCoreMetricDescriptors(deviceObject, Device::MEMORY);
}

QList<MetricDescriptor> DeviceMetricFactory::parseVideoCard(Device *deviceObject)
{
    return createCoreMetricDescriptors(deviceObject, Device::VIDEO_CARD);
}

QList<MetricDescriptor> DeviceMetricFactory::parseHardDisk(Device *deviceObject)
{
    return createLoadingMetricDescriptor(deviceObject, Device::HARD_DISK);
}

QVariant DeviceMetricFactory::processorMetricValue(Device *deviceObject, Metrics::MetricId metricId)
{
    Q_UNUSED(deviceObject)
    Q_UNUSED(metricId)

    return {};
}

QVariant DeviceMetricFactory::memoryMetricValue(Device *deviceObject, Metrics::MetricId metricId)
{
    Q_UNUSED(deviceObject)
    Q_UNUSED(metricId)

    return {};
}

QVariant DeviceMetricFactory::videoCardMetricValue(Device *deviceObject, Metrics::MetricId metricId)
{
    Q_UNUSED(deviceObject)
    Q_UNUSED(metricId)

    return {};
}

QVariant DeviceMetricFactory::hardDiskMetricValue(Device *deviceObject, Metrics::MetricId metricId)
{
    Q_UNUSED(deviceObject)
    Q_UNUSED(metricId)

    return {};
}
