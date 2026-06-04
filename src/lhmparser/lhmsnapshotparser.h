#ifndef LHMSNAPSHOTPARSER_H
#define LHMSNAPSHOTPARSER_H

#include "hardwaresnapshot.h"

#include <QJsonObject>

class LhmSnapshotParser
{
public:
    HardwareSnapshot parse(const QJsonObject &root) const;
};

#endif // LHMSNAPSHOTPARSER_H
