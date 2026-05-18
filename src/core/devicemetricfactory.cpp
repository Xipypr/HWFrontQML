#include "devicemetricfactory.h"

#include "storages/desktopdevice.h"

#include <QMetaObject>

namespace {
struct DeviceMetricDefinition
{
    Metrics::MetricId metricId = Metrics::MetricId::Unknown;
    const char *readerMethod = nullptr;
};

QString displayName(Device *deviceObject, const QString &fallbackDeviceId)
{
    const QString deviceName = deviceObject ? deviceObject->name() : QString();
    return deviceName.isEmpty() ? fallbackDeviceId.toUpper() : deviceName;
}

QVariant invokeMetricReader(Device *deviceObject, const char *readerMethod)
{
    if (!deviceObject || !readerMethod)
        return {};

    QVariant variantValue;
    if (QMetaObject::invokeMethod(deviceObject,
                                  readerMethod,
                                  Qt::DirectConnection,
                                  Q_RETURN_ARG(QVariant, variantValue))) {
        return variantValue;
    }

    int intValue = 0;
    if (QMetaObject::invokeMethod(deviceObject,
                                  readerMethod,
                                  Qt::DirectConnection,
                                  Q_RETURN_ARG(int, intValue))) {
        return intValue;
    }

    double doubleValue = 0.0;
    if (QMetaObject::invokeMethod(deviceObject,
                                  readerMethod,
                                  Qt::DirectConnection,
                                  Q_RETURN_ARG(double, doubleValue))) {
        return doubleValue;
    }

    float floatValue = 0.0F;
    if (QMetaObject::invokeMethod(deviceObject,
                                  readerMethod,
                                  Qt::DirectConnection,
                                  Q_RETURN_ARG(float, floatValue))) {
        return floatValue;
    }

    return {};
}

bool canReadMetric(Device *deviceObject, const char *readerMethod)
{
    return invokeMetricReader(deviceObject, readerMethod).isValid();
}

QList<MetricDescriptor> createDescriptors(Device *deviceObject,
                                          const QString &deviceId,
                                          const QList<DeviceMetricDefinition> &definitions)
{
    QList<MetricDescriptor> descriptors;
    if (!deviceObject || deviceId.isEmpty())
        return descriptors;

    for (const DeviceMetricDefinition &definition : definitions) {
        if (definition.metricId == Metrics::MetricId::Unknown
                || !canReadMetric(deviceObject, definition.readerMethod)) {
            continue;
        }

        descriptors.push_back({
            deviceId,
            definition.metricId,
            displayName(deviceObject, deviceId),
            Metrics::metricUnit(definition.metricId)
        });
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

const char *metricReaderMethod(Device *deviceObject, Metrics::MetricId metricId)
{
    if (!deviceObject || metricId == Metrics::MetricId::Unknown)
        return nullptr;

    const QList<DeviceMetricDefinition> *definitions = nullptr;
    switch (deviceObject->type()) {
    case Device::PROCESSOR:
        definitions = &processorMetricDefinitions();
        break;
    case Device::MEMORY:
        definitions = &memoryMetricDefinitions();
        break;
    case Device::VIDEO_CARD:
        definitions = &videoCardMetricDefinitions();
        break;
    case Device::HARD_DISK:
        definitions = &hardDiskMetricDefinitions();
        break;
    default:
        return nullptr;
    }

    for (const DeviceMetricDefinition &definition : *definitions) {
        if (definition.metricId == metricId)
            return definition.readerMethod;
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
    return invokeMetricReader(deviceObject, metricReaderMethod(deviceObject, metricId));
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
    return createDescriptors(deviceObject, QStringLiteral("cpu"), processorMetricDefinitions());
}

QList<MetricDescriptor> DeviceMetricFactory::parseMemory(Device *deviceObject)
{
    return createDescriptors(deviceObject, QStringLiteral("ram"), memoryMetricDefinitions());
}

QList<MetricDescriptor> DeviceMetricFactory::parseVideoCard(Device *deviceObject)
{
    return createDescriptors(deviceObject, QStringLiteral("gpu"), videoCardMetricDefinitions());
}

QList<MetricDescriptor> DeviceMetricFactory::parseHardDisk(Device *deviceObject)
{
    return createDescriptors(deviceObject, QStringLiteral("hdd"), hardDiskMetricDefinitions());
}
