#include "devicemetricfactory.h"

#include "storages/desktopdevice.h"

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
        return processorDescriptors(deviceObject);
    case Device::MEMORY:
        return memoryDescriptors(deviceObject);
    case Device::VIDEO_CARD:
        return videoCardDescriptors(deviceObject);
    case Device::HARD_DISK:
        return hardDiskDescriptors(deviceObject);
    default:
        return {};
    }
}

QList<MetricDescriptor> DeviceMetricFactory::processorDescriptors(Device *deviceObject)
{
    const QString deviceName = deviceObject->name();
    return {
        MetricDescriptor::createLoadingDescr(QStringLiteral("cpu"), deviceName),
        MetricDescriptor::createTempDescr(QStringLiteral("cpu"), deviceName),
        MetricDescriptor::createFrequencyDescr(QStringLiteral("cpu"), deviceName)
    };
}

QList<MetricDescriptor> DeviceMetricFactory::memoryDescriptors(Device *deviceObject)
{
    const QString deviceName = deviceObject->name();
    return {
        MetricDescriptor::createLoadingDescr(QStringLiteral("ram"), deviceName),
        MetricDescriptor::createTempDescr(QStringLiteral("ram"), deviceName),
        MetricDescriptor::createFrequencyDescr(QStringLiteral("ram"), deviceName)
    };
}

QList<MetricDescriptor> DeviceMetricFactory::videoCardDescriptors(Device *deviceObject)
{
    const QString deviceName = deviceObject->name();
    return {
        MetricDescriptor::createLoadingDescr(QStringLiteral("gpu"), deviceName),
        MetricDescriptor::createTempDescr(QStringLiteral("gpu"), deviceName),
        MetricDescriptor::createFrequencyDescr(QStringLiteral("gpu"), deviceName)
    };
}

QList<MetricDescriptor> DeviceMetricFactory::hardDiskDescriptors(Device *deviceObject)
{
    return {
        MetricDescriptor::createLoadingDescr(QStringLiteral("hdd"), deviceObject->name())
    };
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
