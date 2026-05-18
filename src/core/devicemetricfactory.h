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
    static QList<MetricDescriptor> processorDescriptors(Device *deviceObject);
    static QList<MetricDescriptor> memoryDescriptors(Device *deviceObject);
    static QList<MetricDescriptor> videoCardDescriptors(Device *deviceObject);
    static QList<MetricDescriptor> hardDiskDescriptors(Device *deviceObject);

    static QVariant processorMetricValue(Device *deviceObject, Metrics::MetricId metricId);
    static QVariant memoryMetricValue(Device *deviceObject, Metrics::MetricId metricId);
    static QVariant videoCardMetricValue(Device *deviceObject, Metrics::MetricId metricId);
    static QVariant hardDiskMetricValue(Device *deviceObject, Metrics::MetricId metricId);
};

#endif // DEVICEMETRICFACTORY_H
