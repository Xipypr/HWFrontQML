#ifndef LINUXSNAPSHOTPARSER_H
#define LINUXSNAPSHOTPARSER_H

#include <hardwaresnapshot.h>

#include <QJsonObject>
#include <QString>

class LinuxSnapshotParser
{
public:
    HardwareSnapshot parse(const QJsonObject &root) const;
    QString displayName(const QJsonObject &root) const;
};

#endif // LINUXSNAPSHOTPARSER_H
