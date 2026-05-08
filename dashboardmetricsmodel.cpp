#include "dashboardmetricsmodel.h"

#include <QSet>

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
        return item.widgetId.section(QLatin1Char(':'), 0, 0);
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
        { "title", item.widgetId.section(QLatin1Char(':'), 0, 0) },
        { "value", item.value },
        { "variant", item.variant },
        { "available", item.available },
        { "metricId", Metrics::metricIdToString(item.metricId) },
        { "unit", item.unit }
    };
}

bool DashboardMetricsModel::addWidget(const QString &deviceId,
                                      Metrics::MetricId metricId,
                                      const QString &variant,
                                      bool available,
                                      const QString &unit)
{
    if (deviceId.isEmpty() || metricId == Metrics::MetricId::Unknown || widgetIndexForMetric(deviceId, metricId) >= 0)
        return false;

    const int insertRow = m_items.size();
    beginInsertRows(QModelIndex(), insertRow, insertRow);
    m_items.push_back({
        makeWidgetId(deviceId, metricId),
        0,
        variant,
        available,
        metricId,
        unit.isEmpty() ? Metrics::metricUnit(metricId) : unit
    });
    endInsertRows();
    return true;
}

bool DashboardMetricsModel::addWidgetByType(DashboardMetricsModel::WidgetType type)
{
    const WidgetDescriptor descriptor = descriptorForType(type);
    if (descriptor.type == Unknown)
        return false;

    return addWidget(descriptor.widgetId,
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

bool DashboardMetricsModel::updateWidget(const QString &deviceId,
                                         Metrics::MetricId metricId,
                                         int value,
                                         bool available)
{
    const int index = widgetIndexForMetric(deviceId, metricId);
    if (index < 0)
        return false;

    setWidgetValue(deviceId, metricId, value, available);
    return true;
}


void DashboardMetricsModel::onAvailableMetricsChanged(const QList<MetricDescriptor> &metrics)
{
    syncWidgetsWithMetrics(metrics);
}

void DashboardMetricsModel::onMetricUpdated(const QString &deviceId,
                                            Metrics::MetricId metricId,
                                            const QVariant &value)
{
    const bool available = value.isValid() && !value.isNull() && value.canConvert(QVariant::Int);
    if (!available) {
        setWidgetAvailability(deviceId, metricId, false);
        return;
    }

    setWidgetValue(deviceId, metricId, value.toInt(), true);
}

void DashboardMetricsModel::syncWidgetsWithMetrics(const QList<MetricDescriptor> &metrics)
{
    QSet<QString> availableMetricKeys;

    for (const MetricDescriptor &descriptor : metrics) {
        if (descriptor.deviceId.isEmpty() || descriptor.metricId == Metrics::MetricId::Unknown)
            continue;

        availableMetricKeys.insert(makeWidgetId(descriptor.deviceId, descriptor.metricId));

        if (widgetIndexForMetric(descriptor.deviceId, descriptor.metricId) < 0) {
            addWidget(descriptor.deviceId,
                      descriptor.metricId,
                      QStringLiteral("segments"),
                      true,
                      descriptor.unit);
        } else {
            setWidgetValue(descriptor.deviceId, descriptor.metricId, 0, true, descriptor.unit);
        }
    }

    for (int i = 0; i < m_items.size(); ++i) {
        const WidgetItem &item = m_items.at(i);
        if (!availableMetricKeys.contains(item.widgetId))
            setWidgetAvailability(item.widgetId.section(QLatin1Char(':'), 0, 0), item.metricId, false);
    }
}

QString DashboardMetricsModel::makeWidgetId(const QString &deviceId, Metrics::MetricId metricId)
{
    return deviceId + QStringLiteral(":") + Metrics::metricIdToString(metricId);
}

int DashboardMetricsModel::widgetIndexById(const QString &widgetId) const
{
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items.at(i).widgetId == widgetId)
            return i;
    }

    return -1;
}

int DashboardMetricsModel::widgetIndexForMetric(const QString &deviceId, Metrics::MetricId metricId) const
{
    for (int i = 0; i < m_items.size(); ++i) {
        const WidgetItem &item = m_items.at(i);
        if (item.widgetId == makeWidgetId(deviceId, metricId) && item.metricId == metricId)
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

void DashboardMetricsModel::setWidgetValue(const QString &deviceId,
                                           Metrics::MetricId metricId,
                                           int value,
                                           bool available,
                                           const QString &unit)
{
    const int index = widgetIndexForMetric(deviceId, metricId);
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

void DashboardMetricsModel::setWidgetAvailability(const QString &deviceId, Metrics::MetricId metricId, bool available)
{
    const int index = widgetIndexForMetric(deviceId, metricId);
    if (index < 0)
        return;

    WidgetItem &item = m_items[index];
    if (item.available == available)
        return;

    item.available = available;
    const QModelIndex modelIndex = this->index(index);
    emit dataChanged(modelIndex, modelIndex, { AvailableRole });
}
