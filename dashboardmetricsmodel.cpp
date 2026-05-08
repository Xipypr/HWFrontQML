#include "dashboardmetricsmodel.h"

#include <QSet>

uint qHash(const DashboardMetricWidgetKey &key, uint seed)
{
    const uint deviceHash = ::qHash(key.deviceId, seed);
    const uint metricHash = static_cast<uint>(key.metricId);
    return deviceHash ^ (metricHash + 0x9e3779b9U + (deviceHash << 6) + (deviceHash >> 2));
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
    case DeviceIdRole:
        return item.deviceId;
    case TitleRole:
        return item.title;
    case ValueRole:
        return item.value;
    case VariantRole:
        return item.variant;
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
        { DeviceIdRole, "deviceId" },
        { TitleRole, "title" },
        { ValueRole, "value" },
        { VariantRole, "variant" },
        { MetricIdRole, "metricId" },
        { UnitRole, "unit" }
    };
}

int DashboardMetricsModel::availableMetricsRevision() const
{
    return m_availableMetricsRevision;
}

QVariantMap DashboardMetricsModel::get(int row) const
{
    if (row < 0 || row >= m_items.size())
        return {};

    const WidgetItem &item = m_items.at(row);
    return {
        { "widgetId", item.widgetId },
        { "deviceId", item.deviceId },
        { "title", item.title },
        { "value", item.value },
        { "variant", item.variant },
        { "metricId", Metrics::metricIdToString(item.metricId) },
        { "unit", item.unit }
    };
}

QVariantList DashboardMetricsModel::availableDevices() const
{
    QVariantList devices;
    QSet<QString> seenDeviceIds;

    for (const MetricDescriptor &descriptor : m_availableMetrics) {
        if (descriptor.deviceId.isEmpty() || seenDeviceIds.contains(descriptor.deviceId))
            continue;

        seenDeviceIds.insert(descriptor.deviceId);
        devices.push_back(QVariantMap{
            { "deviceId", descriptor.deviceId },
            { "label", descriptor.displayName.isEmpty() ? descriptor.deviceId : descriptor.displayName }
        });
    }

    return devices;
}

QVariantList DashboardMetricsModel::availableMetricsForDevice(const QString &deviceId) const
{
    QVariantList metrics;

    if (deviceId.isEmpty())
        return metrics;

    for (const MetricDescriptor &descriptor : m_availableMetrics) {
        if (descriptor.deviceId != deviceId || descriptor.metricId == Metrics::MetricId::Unknown)
            continue;

        const QString metricName = Metrics::metricIdToString(descriptor.metricId);
        metrics.push_back(QVariantMap{
            { "metricId", metricName },
            { "label", metricName },
            { "title", descriptor.displayName },
            { "unit", descriptor.unit }
        });
    }

    return metrics;
}

bool DashboardMetricsModel::addWidget(const QString &deviceId,
                                      Metrics::MetricId metricId,
                                      const QString &title,
                                      const QString &unit,
                                      const QString &variant)
{
    const DashboardMetricWidgetKey key = makeWidgetKey(deviceId, metricId);
    if (!key.isValid())
        return false;

    int initialValue = 0;
    const QVariant cachedValue = m_latestMetricValues.value(key);
    if (cachedValue.isValid() && !cachedValue.isNull() && cachedValue.canConvert(QVariant::Int))
        initialValue = cachedValue.toInt();

    return insertWidget({
        makeWidgetId(key),
        key.deviceId,
        title.isEmpty() ? key.deviceId : title,
        initialValue,
        variant.isEmpty() ? QStringLiteral("segments") : variant,
        key.metricId,
        unit.isEmpty() ? Metrics::metricUnit(key.metricId) : unit
    });
}

bool DashboardMetricsModel::addWidgetForMetric(const QString &deviceId,
                                               const QString &metricId,
                                               const QString &variant)
{
    const Metrics::MetricId parsedMetricId = Metrics::metricIdFromString(metricId);
    if (deviceId.isEmpty() || parsedMetricId == Metrics::MetricId::Unknown)
        return false;

    const MetricDescriptor *descriptor = descriptorForMetric(deviceId, parsedMetricId);
    if (!descriptor)
        return false;

    return addWidget(descriptor->deviceId,
                     descriptor->metricId,
                     descriptor->displayName,
                     descriptor->unit,
                     variant);
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

bool DashboardMetricsModel::updateWidget(const QString &deviceId,
                                         Metrics::MetricId metricId,
                                         int value)
{
    const int index = widgetIndexForMetric(deviceId, metricId);
    if (index < 0)
        return false;

    setWidgetValue(deviceId, metricId, value);
    return true;
}

void DashboardMetricsModel::onAvailableMetricsChanged(const QList<MetricDescriptor> &metrics)
{
    m_availableMetrics = metrics;
    syncInitialWidgetsWithMetrics();

    ++m_availableMetricsRevision;
    emit availableMetricsChanged();

    for (const MetricDescriptor &descriptor : m_availableMetrics) {
        if (descriptor.deviceId.isEmpty() || descriptor.metricId == Metrics::MetricId::Unknown)
            continue;

        const int widgetIndex = widgetIndexForMetric(descriptor.deviceId, descriptor.metricId);
        if (widgetIndex < 0)
            continue;

        WidgetItem &item = m_items[widgetIndex];
        if (!descriptor.unit.isEmpty() && item.unit != descriptor.unit) {
            item.unit = descriptor.unit;
            const QModelIndex modelIndex = this->index(widgetIndex);
            emit dataChanged(modelIndex, modelIndex, { UnitRole });
        }
    }
}

void DashboardMetricsModel::onMetricUpdated(const QString &title,
                                            Metrics::MetricId metricId,
                                            const QVariant &value)
{
    if (title.isEmpty() || metricId == Metrics::MetricId::Unknown
            || !value.isValid() || value.isNull() || !value.canConvert(QVariant::Int)) {
        return;
    }

    const MetricDescriptor *descriptor = descriptorForMetricTitle(title, metricId);
    if (!descriptor)
        return;

    const DashboardMetricWidgetKey key = makeWidgetKey(descriptor->deviceId, metricId);
    m_latestMetricValues.insert(key, value);
    setWidgetValue(descriptor->deviceId, metricId, value.toInt());
}

DashboardMetricWidgetKey DashboardMetricsModel::makeWidgetKey(const QString &deviceId, Metrics::MetricId metricId)
{
    return { deviceId, metricId };
}

QString DashboardMetricsModel::makeWidgetId(const DashboardMetricWidgetKey &key)
{
    return key.deviceId + QStringLiteral(":") + Metrics::metricIdToString(key.metricId);
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
    const DashboardMetricWidgetKey key = makeWidgetKey(deviceId, metricId);
    const auto indexIt = m_widgetIndexByKey.constFind(key);
    return indexIt == m_widgetIndexByKey.constEnd() ? -1 : indexIt.value();
}

bool DashboardMetricsModel::insertWidget(const WidgetItem &item)
{
    const DashboardMetricWidgetKey key = makeWidgetKey(item.deviceId, item.metricId);
    if (!key.isValid() || m_widgetIndexByKey.contains(key))
        return false;

    const int insertRow = m_items.size();
    beginInsertRows(QModelIndex(), insertRow, insertRow);
    m_items.push_back(item);
    m_widgetIndexByKey.insert(key, insertRow);
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
    for (int i = 0; i < m_items.size(); ++i) {
        const WidgetItem &item = m_items.at(i);
        const DashboardMetricWidgetKey key = makeWidgetKey(item.deviceId, item.metricId);
        if (!key.isValid())
            continue;

        m_widgetIndexByKey.insert(key, i);
    }
}

void DashboardMetricsModel::setWidgetValue(const QString &deviceId,
                                           Metrics::MetricId metricId,
                                           int value,
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

    if (!unit.isEmpty() && item.unit != unit) {
        item.unit = unit;
        changedRoles.push_back(UnitRole);
    }

    if (changedRoles.isEmpty())
        return;

    const QModelIndex modelIndex = this->index(index);
    emit dataChanged(modelIndex, modelIndex, changedRoles);
}


void DashboardMetricsModel::syncInitialWidgetsWithMetrics()
{
    if (m_hasSeededInitialWidgets)
        return;

    for (const MetricDescriptor &descriptor : m_availableMetrics) {
        if (descriptor.deviceId.isEmpty() || descriptor.metricId == Metrics::MetricId::Unknown)
            continue;

        addWidget(descriptor.deviceId,
                  descriptor.metricId,
                  descriptor.displayName,
                  descriptor.unit,
                  QStringLiteral("segments"));
    }

    if (!m_items.isEmpty())
        m_hasSeededInitialWidgets = true;
}

const MetricDescriptor *DashboardMetricsModel::descriptorForMetric(const QString &deviceId,
                                                                   Metrics::MetricId metricId) const
{
    for (const MetricDescriptor &descriptor : m_availableMetrics) {
        if (descriptor.deviceId == deviceId && descriptor.metricId == metricId)
            return &descriptor;
    }

    return nullptr;
}

const MetricDescriptor *DashboardMetricsModel::descriptorForMetricTitle(const QString &title,
                                                                        Metrics::MetricId metricId) const
{
    for (const MetricDescriptor &descriptor : m_availableMetrics) {
        if (descriptor.displayName == title && descriptor.metricId == metricId)
            return &descriptor;
    }

    return nullptr;
}
