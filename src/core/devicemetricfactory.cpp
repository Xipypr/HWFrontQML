#include "devicemetricfactory.h"

#include "storages/desktopdevice.h"
#include "storages/cpu.h"
#include "storages/hdd.h"
#include "storages/ram.h"
#include "storages/videocard.h"

QList<MetricDescriptor> DeviceMetricFactory::createDescriptors(DesktopDevice *desktopDevice)
{
    QList<MetricDescriptor> descriptors;

    const QList<Device *> devices = desktopDevice->devicesList();
    for (Device *deviceObject : devices)
        descriptors.append(createDescriptorsForDevice(deviceObject));

    return descriptors;
}

QString DeviceMetricFactory::deviceId(Device *deviceObject)
{
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
        MetricDescriptor::createTemperatureDescr(QStringLiteral("cpu"), deviceName),
        // MetricDescriptor::createFrequencyDescr(QStringLiteral("cpu"), deviceName)
    };
}

QList<MetricDescriptor> DeviceMetricFactory::memoryDescriptors(Device *deviceObject)
{
    const QString deviceName = deviceObject->name();
    return {
        MetricDescriptor::createLoadingDescr(QStringLiteral("ram"), deviceName),
        MetricDescriptor::createTemperatureDescr(QStringLiteral("ram"), deviceName),
        // MetricDescriptor::createFrequencyDescr(QStringLiteral("ram"), deviceName)
    };
}

QList<MetricDescriptor> DeviceMetricFactory::videoCardDescriptors(Device *deviceObject)
{
    const QString deviceName = deviceObject->name();
    return {
        MetricDescriptor::createLoadingDescr(QStringLiteral("gpu"), deviceName),
        MetricDescriptor::createTemperatureDescr(QStringLiteral("gpu"), deviceName),
        // MetricDescriptor::createFrequencyDescr(QStringLiteral("gpu"), deviceName)
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
    auto cpu = dynamic_cast<Cpu *>(deviceObject);
    switch (metricId) {
    case Metrics::MetricId::Loading:
        return cpu->loading();

    case Metrics::MetricId::Frequency:
        return {};

    case Metrics::MetricId::Temperature:
        return cpu->temperature();

    case Metrics::MetricId::Unknown:
        return {};
    }

    return {};
}

QVariant DeviceMetricFactory::memoryMetricValue(Device *deviceObject, Metrics::MetricId metricId)
{
    auto ram = dynamic_cast<Ram *>(deviceObject);
    switch (metricId) {
    case Metrics::MetricId::Loading:
        return ram->loading();

    case Metrics::MetricId::Frequency:
    case Metrics::MetricId::Temperature:
    case Metrics::MetricId::Unknown:
        return {};
    }

    return {};
}

QVariant DeviceMetricFactory::videoCardMetricValue(Device *deviceObject, Metrics::MetricId metricId)
{
    auto gpu = dynamic_cast<VideoCard *>(deviceObject);
    switch (metricId) {
    case Metrics::MetricId::Loading:
        return gpu->loading();

    case Metrics::MetricId::Frequency:
        return {};

    case Metrics::MetricId::Temperature:
        return gpu->temperature();

    case Metrics::MetricId::Unknown:
        return {};
    }

    return {};
}

QVariant DeviceMetricFactory::hardDiskMetricValue(Device *deviceObject, Metrics::MetricId metricId)
{
    auto hdd = dynamic_cast<Hdd *>(deviceObject);
    switch (metricId) {
    case Metrics::MetricId::Loading:
        return hdd->loadValue();

    case Metrics::MetricId::Temperature:
        return hdd->temperatureValue();

    case Metrics::MetricId::Frequency:
    case Metrics::MetricId::Unknown:
        return {};
    }

    return {};
}
