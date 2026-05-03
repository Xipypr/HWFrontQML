#include "dashboardmetricsmodel.h"

DashboardMetricsModel::DashboardMetricsModel(QObject *parent)
    : QAbstractListModel(parent)
{
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
                                      const QString &title,
                                      int value,
                                      const QString &variant,
                                      bool available)
{
    if (widgetId.isEmpty() || findWidgetIndex(widgetId) >= 0)
        return false;

    const int insertRow = m_items.size();
    beginInsertRows(QModelIndex(), insertRow, insertRow);
    m_items.push_back({ widgetId, title, value, variant, available });
    endInsertRows();
    return true;
}


bool DashboardMetricsModel::addWidgetByType(const QString &type)
{
    if (type == "cpu")
        return addWidget("cpu", "CPU", 0, "segments", true);
    if (type == "ram")
        return addWidget("ram", "RAM", 0, "ring", true);
    if (type == "gpu")
        return addWidget("gpu", "GPU", 0, "linear", true);
    if (type == "hdd")
        return addWidget("hdd", "HDD", 0, "arc180", true);


    return false;
}


QVariantList DashboardMetricsModel::widgetTypeOptions() const
{
    return {
        QVariantMap{{"label", "CPU"}, {"key", "cpu"}},
        QVariantMap{{"label", "RAM"}, {"key", "ram"}},
        QVariantMap{{"label", "GPU"}, {"key", "gpu"}},
        QVariantMap{{"label", "HDD (180°)"}, {"key", "hdd"}}
    };
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
                                         const QString &title,
                                         int value,
                                         bool available)
{
    const int index = findWidgetIndex(widgetId);
    if (index < 0)
        return false;

    WidgetItem &item = m_items[index];
    item.title = title;
    item.value = value;
    item.available = available;

    const QModelIndex modelIndex = this->index(index);
    emit dataChanged(modelIndex, modelIndex, { TitleRole, ValueRole, AvailableRole });
    return true;
}

int DashboardMetricsModel::findWidgetIndex(const QString &widgetId) const
{
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items.at(i).widgetId == widgetId)
            return i;
    }

    return -1;
}

