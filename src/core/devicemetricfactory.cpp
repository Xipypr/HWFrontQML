#include "devicemetricfactory.h"

#include "storages/desktopdevice.h"

namespace {
QString displayName(Device *deviceObject, const QString &fallbackDeviceId)
{
    const QString deviceName = deviceObject ? deviceObject->name() : QString();
    return deviceName.isEmpty() ? fallbackDeviceId.toUpper() : deviceName;
}

QList<MetricDescriptor> createCoreMetricDescriptors(Device *deviceObject, const QString &deviceId)
{
    const QString name = displayName(deviceObject, deviceId);
    return {
        MetricDescriptor::createLoadingDescr(deviceId, name),
        MetricDescriptor::createTempDescr(deviceId, name),
        MetricDescriptor::createFrequencyDescr(deviceId, name)
    };
}

QList<MetricDescriptor> createLoadingMetricDescriptor(Device *deviceObject, const QString &deviceId)
{
    return {
        MetricDescriptor::createLoadingDescr(deviceId, displayName(deviceObject, deviceId))
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
    return createCoreMetricDescriptors(deviceObject, QStringLiteral("cpu"));
}

QList<MetricDescriptor> DeviceMetricFactory::parseMemory(Device *deviceObject)
{
    return createCoreMetricDescriptors(deviceObject, QStringLiteral("ram"));
}

QList<MetricDescriptor> DeviceMetricFactory::parseVideoCard(Device *deviceObject)
{
    return createCoreMetricDescriptors(deviceObject, QStringLiteral("gpu"));
}

QList<MetricDescriptor> DeviceMetricFactory::parseHardDisk(Device *deviceObject)
{
    return createLoadingMetricDescriptor(deviceObject, QStringLiteral("hdd"));
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
