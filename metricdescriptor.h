#ifndef METRICDESCRIPTOR_H
#define METRICDESCRIPTOR_H

#include <QList>
#include <QMetaType>
#include <QString>

struct MetricDescriptor
{
    QString deviceId;
    QString metricId;
    QString displayName;
    QString unit;
};

Q_DECLARE_METATYPE(MetricDescriptor)
Q_DECLARE_METATYPE(QList<MetricDescriptor>)

#endif // METRICDESCRIPTOR_H
