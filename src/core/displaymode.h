#ifndef DISPLAYMODE_H
#define DISPLAYMODE_H

#include <QMetaType>
#include <QObject>

namespace DashboardDisplay {
Q_NAMESPACE

enum class Mode {
    Segments = 0,
    Ring,
    Linear,
    Arc180,
    NetworkVertical,
    NetworkHorizontal
};
Q_ENUM_NS(Mode)

inline bool isNetworkMode(Mode mode)
{
    return mode == Mode::NetworkVertical || mode == Mode::NetworkHorizontal;
}

}

Q_DECLARE_METATYPE(DashboardDisplay::Mode)

#endif // DISPLAYMODE_H
