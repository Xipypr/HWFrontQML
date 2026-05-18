#include "dashboardmetricsmodel.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QSet>

namespace {
QString deviceTypeToId(DeviceMetricType deviceType)
{
    switch (deviceType) {
    case Device::PROCESSOR:
        return QStringLiteral("cpu");
    case Device::MEMORY:
        return QStringLiteral("ram");
    case Device::VIDEO_CARD:
        return QStringLiteral("gpu");
    case Device::HARD_DISK:
        return QStringLiteral("hdd");
    default:
        return {};
    }
}

DeviceMetricType deviceTypeFromId(const QString &deviceId)
{
    if (deviceId == QStringLiteral("cpu"))
        return Device::PROCESSOR;
    if (deviceId == QStringLiteral("ram"))
        return Device::MEMORY;
    if (deviceId == QStringLiteral("gpu"))
        return Device::VIDEO_CARD;
    if (deviceId == QStringLiteral("hdd"))
        return Device::HARD_DISK;

    return DeviceMetricType{};
}
}

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
        { "metricId", Metrics::metricIdToString(item.metricId) },
        { "unit", item.unit }
    };
}

QJsonArray DashboardMetricsModel::toJson() const
{
    QJsonArray widgetsArray;

    for (const WidgetItem &item : m_items) {
        QJsonObject widgetObject;
        widgetObject[QStringLiteral("title")] = item.title;
        widgetObject[QStringLiteral("metricId")] = Metrics::metricIdToString(item.metricId);
        widgetObject[QStringLiteral("unit")] = item.unit;
        widgetObject[QStringLiteral("variant")] = item.variant;
        widgetsArray.append(widgetObject);
    }

    return widgetsArray;
}

bool DashboardMetricsModel::hasSeededInitialWidgets() const
{
    return m_hasSeededInitialWidgets;
}

void DashboardMetricsModel::restoreFromJson(const QJsonArray &widgets)
{
    QVector<WidgetItem> restoredItems;
    QHash<DashboardMetricWidgetKey, int> restoredIndexes;

    for (const QJsonValue &value : widgets) {
        if (!value.isObject())
            continue;

        const QJsonObject widgetObject = value.toObject();
        const QString title = widgetObject.value(QStringLiteral("title")).toString().trimmed();
        const Metrics::MetricId metricId = Metrics::metricIdFromString(widgetObject.value(QStringLiteral("metricId")).toString());
        const DashboardMetricWidgetKey key = makeWidgetKey(title, metricId);

        if (!key.isValid() || restoredIndexes.contains(key))
            continue;

        WidgetItem item;
        item.widgetId = makeWidgetId(key);
        item.title = key.title;
        item.value = 0;
        item.variant = widgetObject.value(QStringLiteral("variant")).toString(QStringLiteral("segments"));
        if (item.variant.isEmpty())
            item.variant = QStringLiteral("segments");
        item.metricId = key.metricId;
        item.unit = widgetObject.value(QStringLiteral("unit")).toString();
        if (item.unit.isEmpty())
            item.unit = Metrics::metricUnit(key.metricId);

        restoredIndexes.insert(key, restoredItems.size());
        restoredItems.push_back(item);
    }

    beginResetModel();
    m_items = restoredItems;
    m_widgetIndexByKey = restoredIndexes;
    m_hasSeededInitialWidgets = !m_items.isEmpty();
    endResetModel();
}

QVariantList DashboardMetricsModel::availableDevices() const
{
    QVariantList devices;
    QSet<QString> seenDeviceIds;

    for (const MetricDescriptor &descriptor : m_availableMetrics) {
        const QString deviceId = deviceTypeToId(descriptor.deviceType);
        if (deviceId.isEmpty() || seenDeviceIds.contains(deviceId))
            continue;

        seenDeviceIds.insert(deviceId);
        devices.push_back(QVariantMap{
            { "deviceId", deviceId },
            { "label", descriptor.displayName.isEmpty() ? deviceId : descriptor.displayName }
        });
    }

    return devices;
}

QVariantList DashboardMetricsModel::availableMetricsForDevice(const QString &deviceId) const
{
    QVariantList metrics;

    const DeviceMetricType deviceType = deviceTypeFromId(deviceId);
    if (!isValidDeviceMetricType(deviceType))
        return metrics;

    for (const MetricDescriptor &descriptor : m_availableMetrics) {
        if (descriptor.deviceType != deviceType || descriptor.metricId == Metrics::MetricId::Unknown)
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

bool DashboardMetricsModel::addWidget(const QString &title,
                                      Metrics::MetricId metricId,
                                      const QString &unit,
                                      const QString &variant)
{
    const DashboardMetricWidgetKey key = makeWidgetKey(title, metricId);
    if (!key.isValid())
        return false;

    return insertWidget({
        makeWidgetId(key),
        key.title,
        0,
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

    return addWidget(descriptor->displayName,
                     descriptor->metricId,
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
    emit widgetsStateChanged();
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
    emit widgetsStateChanged();
    return true;
}

bool DashboardMetricsModel::updateWidget(const QString &title,
                                         Metrics::MetricId metricId,
                                         int value)
{
    const int index = widgetIndexForMetric(title, metricId);
    if (index < 0)
        return false;

    setWidgetValue(title, metricId, value);
    return true;
}

void DashboardMetricsModel::onAvailableMetricsChanged(const QList<MetricDescriptor> &metrics)
{
    m_availableMetrics = metrics;
    syncInitialWidgetsWithMetrics();

    for (const MetricDescriptor &descriptor : m_availableMetrics) {
        if (!isValidDeviceMetricType(descriptor.deviceType) || descriptor.metricId == Metrics::MetricId::Unknown)
            continue;

        const int widgetIndex = widgetIndexForMetric(descriptor.displayName, descriptor.metricId);
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
            || !value.isValid() || value.isNull() || !value.canConvert<int>()) {
        return;
    }

    setWidgetValue(title, metricId, value.toInt());
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
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items.at(i).widgetId == widgetId)
            return i;
    }

    return -1;
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
    endInsertRows();
    emit widgetsStateChanged();
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
    emit widgetsStateChanged();
    return true;
}

void DashboardMetricsModel::rebuildWidgetIndexes()
{
    m_widgetIndexByKey.clear();
    for (int i = 0; i < m_items.size(); ++i) {
        const WidgetItem &item = m_items.at(i);
        const DashboardMetricWidgetKey key = makeWidgetKey(item.title, item.metricId);
        if (!key.isValid())
            continue;

        m_widgetIndexByKey.insert(key, i);
    }
}

void DashboardMetricsModel::setWidgetValue(const QString &title,
                                           Metrics::MetricId metricId,
                                           int value,
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
        if (!isValidDeviceMetricType(descriptor.deviceType) || descriptor.metricId == Metrics::MetricId::Unknown)
            continue;

        addWidget(descriptor.displayName,
                  descriptor.metricId,
                  descriptor.unit,
                  QStringLiteral("segments"));
    }

    if (!m_items.isEmpty())
        m_hasSeededInitialWidgets = true;
}

const MetricDescriptor *DashboardMetricsModel::descriptorForMetric(const QString &deviceId,
                                                                   Metrics::MetricId metricId) const
{
    const DeviceMetricType deviceType = deviceTypeFromId(deviceId);
    for (const MetricDescriptor &descriptor : m_availableMetrics) {
        if (descriptor.deviceType == deviceType && descriptor.metricId == metricId)
            return &descriptor;
    }

    return nullptr;
}
