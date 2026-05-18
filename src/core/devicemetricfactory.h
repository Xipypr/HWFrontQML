#ifndef DEVICEMETRICFACTORY_H
#define DEVICEMETRICFACTORY_H

#include "metricdescriptor.h"

#include <QVariant>

class DesktopDevice;
class Device;

class DeviceMetricFactory
{
public:
    static QList<MetricDescriptor> createDescriptors(DesktopDevice *desktopDevice);
    static QString deviceId(Device *deviceObject);
    static QVariant metricValue(Device *deviceObject, Metrics::MetricId metricId);

private:
    static QList<MetricDescriptor> createDescriptorsForDevice(Device *deviceObject);
    static QList<MetricDescriptor> parseProcessor(Device *deviceObject);
    static QList<MetricDescriptor> parseMemory(Device *deviceObject);
    static QList<MetricDescriptor> parseVideoCard(Device *deviceObject);
    static QList<MetricDescriptor> parseHardDisk(Device *deviceObject);

    static QVariant processorMetricValue(Device *deviceObject, Metrics::MetricId metricId);
    static QVariant memoryMetricValue(Device *deviceObject, Metrics::MetricId metricId);
    static QVariant videoCardMetricValue(Device *deviceObject, Metrics::MetricId metricId);
    static QVariant hardDiskMetricValue(Device *deviceObject, Metrics::MetricId metricId);
};

#endif // DEVICEMETRICFACTORY_H
