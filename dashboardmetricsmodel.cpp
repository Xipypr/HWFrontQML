#include "sessionmanager.h"
#include "devicebuilder.h"

#include "dashboardmetricsmodel.h"

DashboardMetricsModel::DashboardMetricsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QObject *DashboardMetricsModel::sessionManager() const
{
    return m_sessionManager;
}

void DashboardMetricsModel::setSessionManager(QObject *sessionManager)
{
    if (m_sessionManager == sessionManager)
        return;

    if (m_sessionManager) {
        disconnect(m_sessionManager, nullptr, this, nullptr);
    }

    m_sessionManager = sessionManager;

    if (m_sessionManager) {
        connect(m_sessionManager, SIGNAL(deviceReady(QString,DesktopDevice*)),
                this, SLOT(onDeviceReady(QString,DesktopDevice*)));
    }

    emit sessionManagerChanged();
}

QString DashboardMetricsModel::sessionId() const
{
    return m_sessionId;
}

void DashboardMetricsModel::setSessionId(const QString &sessionId)
{
    if (m_sessionId == sessionId)
        return;

    m_sessionId = sessionId;
    emit sessionIdChanged();
}

int DashboardMetricsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_items.size();
}

QVariant DashboardMetricsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())
        return {};

    const WidgetItem &item = m_items.at(index.row());

    switch (role) {
    case WidgetIdRole:
        return item.widgetId;
    case TitleRole:
        return item.title;
    case ValueRole:
        return item.value;
    case VariantRole:
        return item.variant;
    case AvailableRole:
        return item.available;
    default:
        return {};
    }
}

QHash<int, QByteArray> DashboardMetricsModel::roleNames() const
{
    return {
        { WidgetIdRole, "widgetId" },
        { TitleRole, "title" },
        { ValueRole, "value" },
        { VariantRole, "variant" },
        { AvailableRole, "available" }
    };
}

QVariantMap DashboardMetricsModel::get(int row) const
{
    if (row < 0 || row >= m_items.size())
        return {};

    const WidgetItem &item = m_items.at(row);
    return {
        { "widgetId", item.widgetId },
        { "title", item.title },
        { "value", item.value },
        { "variant", item.variant },
        { "available", item.available }
    };
}

bool DashboardMetricsModel::addWidget(const QString &widgetId,
                                      const QString &variant,
                                      bool available)
{
    if (widgetId.isEmpty() || findWidgetIndex(widgetId) >= 0)
        return false;

    const int insertRow = m_items.size();
    beginInsertRows(QModelIndex(), insertRow, insertRow);
    m_items.push_back({ widgetId, "unknown", 0, variant, available });
    endInsertRows();
    return true;
}

bool DashboardMetricsModel::addWidgetByType(DashboardMetricsModel::WidgetType type)
{
    const WidgetDescriptor descriptor = descriptorForType(type);
    if (descriptor.type == Unknown)
        return false;

    if (findWidgetIndex(descriptor.widgetId) >= 0)
        return false;

    const int insertRow = m_items.size();
    beginInsertRows(QModelIndex(), insertRow, insertRow);
    m_items.push_back({ descriptor.widgetId, descriptor.title, 0, descriptor.variant, true });
    endInsertRows();
    return true;
}

QVariantList DashboardMetricsModel::widgetTypeOptions() const
{
    const QList<WidgetType> types = { Cpu, Ram, Gpu, Hdd };
    QVariantList options;
    for (WidgetType type : types) {
        const WidgetDescriptor descriptor = descriptorForType(type);
        options.push_back(QVariantMap{
            { "label", descriptor.title == "HDD" ? "HDD (180°)" : descriptor.title },
            { "key", static_cast<int>(type) }
        });
    }

    return options;
}

bool DashboardMetricsModel::removeWidget(const QString &widgetId)
{
    const int index = findWidgetIndex(widgetId);
    if (index < 0)
        return false;

    beginRemoveRows(QModelIndex(), index, index);
    m_items.removeAt(index);
    endRemoveRows();
    return true;
}

bool DashboardMetricsModel::moveWidget(int from, int to)
{
    if (from < 0 || to < 0 || from >= m_items.size() || to >= m_items.size() || from == to)
        return false;

    const int destinationRow = from < to ? to + 1 : to;
    if (!beginMoveRows(QModelIndex(), from, from, QModelIndex(), destinationRow))
        return false;

    m_items.move(from, to);
    endMoveRows();
    return true;
}

bool DashboardMetricsModel::setVariant(const QString &widgetId, const QString &variant)
{
    const int index = findWidgetIndex(widgetId);
    if (index < 0)
        return false;

    WidgetItem &item = m_items[index];
    if (item.variant == variant)
        return true;

    item.variant = variant;
    const QModelIndex modelIndex = this->index(index);
    emit dataChanged(modelIndex, modelIndex, { VariantRole });
    return true;
}

bool DashboardMetricsModel::updateWidget(const QString &widgetId,
                                         int value,
                                         bool available)
{
    const int index = findWidgetIndex(widgetId);
    if (index < 0)
        return false;

    WidgetItem &item = m_items[index];
    item.value = value;
    item.available = available;

    const QModelIndex modelIndex = this->index(index);
    emit dataChanged(modelIndex, modelIndex, { ValueRole, AvailableRole });
    return true;
}


void DashboardMetricsModel::onDeviceReady(const QString &sessionId, DesktopDevice *deviceRef)
{
    if (m_sessionId.isEmpty() || m_sessionId != sessionId || !deviceRef)
        return;

    applyDeviceSnapshot(deviceRef->devicesList());
}

void DashboardMetricsModel::applyDeviceSnapshot(const QList<Device *> &devices)
{
    bool hasCpu = false;
    bool hasRam = false;
    bool hasGpu = false;
    bool hasHdd = false;

    for (Device *deviceObject : devices) {
        if (!deviceObject)
            continue;

        const int type = deviceObject->property("type").toInt();
        const int loading = deviceObject->property("loading").toInt();
        const QString className = QString::fromLatin1(deviceObject->metaObject()->className()).toLower();

        const bool isCpu = (type == 1 || type == 2 || className.contains("processor") || className.contains("cpu"));
        const bool isRam = (type == 2 || type == 3 || className.contains("memory") || className.contains("ram"));
        const bool isGpu = (type == 3 || type == 4 || className.contains("video") || className.contains("gpu"));
        const bool isHdd = (type == 4 || type == 5 || className.contains("disk") || className.contains("hdd"));

        if (isCpu) {
            setWidgetValue("cpu", loading, true);
            hasCpu = true;
            continue;
        }

        if (isRam) {
            setWidgetValue("ram", loading, true);
            hasRam = true;
            continue;
        }

        if (isGpu) {
            setWidgetValue("gpu", loading, true);
            hasGpu = true;
            continue;
        }

        if (isHdd) {
            setWidgetValue("hdd", loading, true);
            hasHdd = true;
        }
    }

    setWidgetValue("cpu", 0, hasCpu);
    setWidgetValue("ram", 0, hasRam);
    setWidgetValue("gpu", 0, hasGpu);
    setWidgetValue("hdd", 0, hasHdd);
}

int DashboardMetricsModel::findWidgetIndex(const QString &widgetId) const
{
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items.at(i).widgetId == widgetId)
            return i;
    }

    return -1;
}

DashboardMetricsModel::WidgetDescriptor DashboardMetricsModel::descriptorForType(DashboardMetricsModel::WidgetType type) const
{
    switch (type) {
    case Cpu:
        return { Cpu, "cpu", "CPU", "segments" };
    case Ram:
        return { Ram, "ram", "RAM", "segments" };
    case Gpu:
        return { Gpu, "gpu", "GPU", "segments" };
    case Hdd:
        return { Hdd, "hdd", "HDD", "segments" };
    default:
        return {};
    }
}

void DashboardMetricsModel::setWidgetValue(const QString &widgetId, int value, bool available)
{
    const int index = findWidgetIndex(widgetId);
    if (index < 0)
        return;

    WidgetItem &item = m_items[index];
    if (item.value == value && item.available == available)
        return;

    item.value = value;
    item.available = available;

    const QModelIndex modelIndex = this->index(index);
    emit dataChanged(modelIndex, modelIndex, { ValueRole, AvailableRole });
}
