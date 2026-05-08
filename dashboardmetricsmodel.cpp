#include "dashboardmetricsmodel.h"

#include <QSet>

uint qHash(const DashboardMetricWidgetKey &key, uint seed)
{
    const uint titleHash = ::qHash(key.title, seed);
    const uint metricHash = static_cast<uint>(key.metricId);
    return titleHash ^ (metricHash + 0x9e3779b9U + (titleHash << 6) + (titleHash >> 2));
}

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
    case MetricIdRole:
        return Metrics::metricIdToString(item.metricId);
    case UnitRole:
        return item.unit;
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
        { AvailableRole, "available" },
        { MetricIdRole, "metricId" },
        { UnitRole, "unit" }
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
        { "available", item.available },
        { "metricId", Metrics::metricIdToString(item.metricId) },
        { "unit", item.unit }
    };
}

bool DashboardMetricsModel::addWidget(const QString &title,
                                      Metrics::MetricId metricId,
                                      const QString &variant,
                                      bool available,
                                      const QString &unit)
{
    const DashboardMetricWidgetKey key = makeWidgetKey(title, metricId);
    if (!key.isValid())
        return false;

    return insertWidget({
        makeWidgetId(key),
        key.title,
        0,
        variant,
        available,
        key.metricId,
        unit.isEmpty() ? Metrics::metricUnit(key.metricId) : unit
    });
}

bool DashboardMetricsModel::addWidgetByType(DashboardMetricsModel::WidgetType type)
{
    const WidgetDescriptor descriptor = descriptorForType(type);
    if (descriptor.type == Unknown)
        return false;

    return addWidget(descriptor.title,
                     Metrics::MetricId::Loading,
                     descriptor.variant,
                     false,
                     Metrics::metricUnit(Metrics::MetricId::Loading));
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
    const int index = widgetIndexById(widgetId);
    if (index < 0)
        return false;

    return removeWidgetAt(index);
}

bool DashboardMetricsModel::moveWidget(int from, int to)
{
    if (from < 0 || to < 0 || from >= m_items.size() || to >= m_items.size() || from == to)
        return false;

    const int destinationRow = from < to ? to + 1 : to;
    if (!beginMoveRows(QModelIndex(), from, from, QModelIndex(), destinationRow))
        return false;

    m_items.move(from, to);
    rebuildWidgetIndexes();
    endMoveRows();
    return true;
}

bool DashboardMetricsModel::setVariant(const QString &widgetId, const QString &variant)
{
    const int index = widgetIndexById(widgetId);
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

bool DashboardMetricsModel::updateWidget(const QString &title,
                                         Metrics::MetricId metricId,
                                         int value,
                                         bool available)
{
    const int index = widgetIndexForMetric(title, metricId);
    if (index < 0)
        return false;

    setWidgetValue(title, metricId, value, available);
    return true;
}


void DashboardMetricsModel::onAvailableMetricsChanged(const QList<MetricDescriptor> &metrics)
{
    syncWidgetsWithMetrics(metrics);
}

void DashboardMetricsModel::onMetricUpdated(const QString &title,
                                            Metrics::MetricId metricId,
                                            const QVariant &value)
{
    const bool available = value.isValid() && !value.isNull() && value.canConvert(QVariant::Int);
    if (!available) {
        setWidgetAvailability(title, metricId, false);
        return;
    }

    setWidgetValue(title, metricId, value.toInt(), true);
}

void DashboardMetricsModel::syncWidgetsWithMetrics(const QList<MetricDescriptor> &metrics)
{
    QSet<DashboardMetricWidgetKey> availableMetricKeys;

    for (const MetricDescriptor &descriptor : metrics) {
        if (descriptor.deviceId.isEmpty() || descriptor.metricId == Metrics::MetricId::Unknown)
            continue;

        availableMetricKeys.insert(makeWidgetKey(descriptor.displayName, descriptor.metricId));

        if (widgetIndexForMetric(descriptor.displayName, descriptor.metricId) < 0) {
            addWidget(descriptor.displayName,
                      descriptor.metricId,
                      QStringLiteral("segments"),
                      true,
                      descriptor.unit);
        } else {
            setWidgetValue(descriptor.displayName, descriptor.metricId, 0, true, descriptor.unit);
        }
    }

    for (int i = 0; i < m_items.size(); ++i) {
        const WidgetItem &item = m_items.at(i);
        if (!availableMetricKeys.contains(makeWidgetKey(item.title, item.metricId)))
            setWidgetAvailability(item.title, item.metricId, false);
    }
}

DashboardMetricWidgetKey DashboardMetricsModel::makeWidgetKey(const QString &title, Metrics::MetricId metricId)
{
    return { title, metricId };
}

QString DashboardMetricsModel::makeWidgetId(const DashboardMetricWidgetKey &key)
{
    return key.title + QStringLiteral(":") + Metrics::metricIdToString(key.metricId);
}

int DashboardMetricsModel::widgetIndexById(const QString &widgetId) const
{
    const auto keyIt = m_widgetKeysById.constFind(widgetId);
    if (keyIt == m_widgetKeysById.constEnd())
        return -1;

    const auto indexIt = m_widgetIndexByKey.constFind(keyIt.value());
    return indexIt == m_widgetIndexByKey.constEnd() ? -1 : indexIt.value();
}

int DashboardMetricsModel::widgetIndexForMetric(const QString &title, Metrics::MetricId metricId) const
{
    const DashboardMetricWidgetKey key = makeWidgetKey(title, metricId);
    const auto indexIt = m_widgetIndexByKey.constFind(key);
    return indexIt == m_widgetIndexByKey.constEnd() ? -1 : indexIt.value();
}

bool DashboardMetricsModel::insertWidget(const WidgetItem &item)
{
    const DashboardMetricWidgetKey key = makeWidgetKey(item.title, item.metricId);
    if (!key.isValid() || m_widgetIndexByKey.contains(key))
        return false;

    const int insertRow = m_items.size();
    beginInsertRows(QModelIndex(), insertRow, insertRow);
    m_items.push_back(item);
    m_widgetIndexByKey.insert(key, insertRow);
    m_widgetKeysById.insert(item.widgetId, key);
    endInsertRows();
    return true;
}

bool DashboardMetricsModel::removeWidgetAt(int index)
{
    if (index < 0 || index >= m_items.size())
        return false;

    beginRemoveRows(QModelIndex(), index, index);
    m_items.removeAt(index);
    rebuildWidgetIndexes();
    endRemoveRows();
    return true;
}

void DashboardMetricsModel::rebuildWidgetIndexes()
{
    m_widgetIndexByKey.clear();
    m_widgetKeysById.clear();

    for (int i = 0; i < m_items.size(); ++i) {
        const WidgetItem &item = m_items.at(i);
        const DashboardMetricWidgetKey key = makeWidgetKey(item.title, item.metricId);
        if (!key.isValid())
            continue;

        m_widgetIndexByKey.insert(key, i);
        m_widgetKeysById.insert(item.widgetId, key);
    }
}

DashboardMetricsModel::WidgetDescriptor DashboardMetricsModel::descriptorForType(DashboardMetricsModel::WidgetType type) const
{
    switch (type) {
    case Cpu:
        return { Cpu, "CPU", "segments" };
    case Ram:
        return { Ram, "RAM", "segments" };
    case Gpu:
        return { Gpu, "GPU", "segments" };
    case Hdd:
        return { Hdd, "HDD", "segments" };
    default:
        return {};
    }
}

void DashboardMetricsModel::setWidgetValue(const QString &title,
                                           Metrics::MetricId metricId,
                                           int value,
                                           bool available,
                                           const QString &unit)
{
    const int index = widgetIndexForMetric(title, metricId);
    if (index < 0)
        return;

    WidgetItem &item = m_items[index];
    QVector<int> changedRoles;

    if (item.value != value) {
        item.value = value;
        changedRoles.push_back(ValueRole);
    }

    if (item.available != available) {
        item.available = available;
        changedRoles.push_back(AvailableRole);
    }

    if (!unit.isEmpty() && item.unit != unit) {
        item.unit = unit;
        changedRoles.push_back(UnitRole);
    }

    if (changedRoles.isEmpty())
        return;

    const QModelIndex modelIndex = this->index(index);
    emit dataChanged(modelIndex, modelIndex, changedRoles);
}

void DashboardMetricsModel::setWidgetAvailability(const QString &title, Metrics::MetricId metricId, bool available)
{
    const int index = widgetIndexForMetric(title, metricId);
    if (index < 0)
        return;

    WidgetItem &item = m_items[index];
    if (item.available == available)
        return;

    item.available = available;
    const QModelIndex modelIndex = this->index(index);
    emit dataChanged(modelIndex, modelIndex, { AvailableRole });
}
