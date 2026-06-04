#include "dashboardmetricsmodel.h"

#include <QJsonArray>
#include <QJsonObject>
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
        return item.title;
    case ValueRole:
        return item.value;
    case VariantRole:
        return item.variant;
    case MetricIdRole:
        return Metrics::metricIdToString(item.metricId);
    case UnitRole:
        return item.unit;
    case ShowProgressBarRole:
        return item.showProgressBar;
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
        { UnitRole, "unit" },
        { ShowProgressBarRole, "showProgressBar" }
    };
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
        { "unit", item.unit },
        { "showProgressBar", item.showProgressBar }
    };
}

QJsonArray DashboardMetricsModel::toJson() const
{
    QJsonArray widgetsArray;

    for (const WidgetItem &item : m_items) {
        QJsonObject widgetObject;
        widgetObject[QStringLiteral("deviceId")] = item.deviceId;
        widgetObject[QStringLiteral("title")] = item.title;
        widgetObject[QStringLiteral("metricId")] = Metrics::metricIdToString(item.metricId);
        widgetObject[QStringLiteral("unit")] = item.unit;
        widgetObject[QStringLiteral("showProgressBar")] = item.showProgressBar;
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
    QHash<QString, int> restoredIndexes;

    for (const QJsonValue &value : widgets) {
        if (!value.isObject())
            continue;

        const QJsonObject widgetObject = value.toObject();
        const QString deviceId = widgetObject.value(QStringLiteral("deviceId")).toString().trimmed();
        const QString title = widgetObject.value(QStringLiteral("title")).toString().trimmed();
        const Metrics::MetricId metricId = Metrics::metricIdFromString(widgetObject.value(QStringLiteral("metricId")).toString());
        const QString widgetId = makeWidgetId(deviceId, metricId);

        if (widgetId.isEmpty() || restoredIndexes.contains(widgetId))
            continue;

        WidgetItem item;
        item.widgetId = widgetId;
        item.deviceId = deviceId;
        item.title = title;
        item.value = 0.0;
        item.variant = widgetObject.value(QStringLiteral("variant")).toString(QStringLiteral("segments"));
        if (item.variant.isEmpty())
            item.variant = QStringLiteral("segments");
        item.metricId = metricId;
        item.unit = widgetObject.value(QStringLiteral("unit")).toString();
        if (item.unit.isEmpty())
            item.unit = Metrics::metricUnit(metricId);
        item.showProgressBar = widgetObject.value(QStringLiteral("showProgressBar")).toBool(metricId == Metrics::MetricId::Loading);

        restoredIndexes.insert(widgetId, restoredItems.size());
        restoredItems.push_back(item);
    }

    beginResetModel();
    m_items = restoredItems;
    m_widgetIndexById = restoredIndexes;
    m_hasSeededInitialWidgets = !m_items.isEmpty();
    endResetModel();
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
            { "unit", descriptor.unit },
            { "showProgressBar", descriptor.showProgressBar }
        });
    }

    return metrics;
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

    return addWidget(*descriptor, variant);
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

bool DashboardMetricsModel::updateWidget(const QString &deviceId,
                                         Metrics::MetricId metricId,
                                         double value)
{
    return setWidgetValue(deviceId, metricId, value);
}

void DashboardMetricsModel::onAvailableMetricsChanged(const QList<MetricDescriptor> &metrics)
{
    m_availableMetrics = metrics;
    syncInitialWidgetsWithMetrics();

    for (const MetricDescriptor &descriptor : m_availableMetrics) {
        if (descriptor.deviceId.isEmpty() || descriptor.metricId == Metrics::MetricId::Unknown)
            continue;

        const int widgetIndex = widgetIndexForMetric(descriptor.deviceId, descriptor.metricId);
        if (widgetIndex < 0)
            continue;

        WidgetItem &item = m_items[widgetIndex];
        QVector<int> changedRoles;

        if (item.title != descriptor.displayName) {
            item.title = descriptor.displayName;
            changedRoles.push_back(TitleRole);
        }

        if (!descriptor.unit.isEmpty() && item.unit != descriptor.unit) {
            item.unit = descriptor.unit;
            changedRoles.push_back(UnitRole);
        }

        if (item.showProgressBar != descriptor.showProgressBar) {
            item.showProgressBar = descriptor.showProgressBar;
            changedRoles.push_back(ShowProgressBarRole);
        }

        if (!changedRoles.isEmpty()) {
            const QModelIndex modelIndex = this->index(widgetIndex);
            emit dataChanged(modelIndex, modelIndex, changedRoles);
        }
    }
}

void DashboardMetricsModel::onMetricUpdated(const QString &deviceId,
                                            Metrics::MetricId metricId,
                                            const QVariant &value)
{
    if (deviceId.isEmpty() || metricId == Metrics::MetricId::Unknown
            || !value.isValid() || value.isNull() || !value.canConvert<double>()) {
        return;
    }

    setWidgetValue(deviceId, metricId, value.toDouble());
}

bool DashboardMetricsModel::isMetricKeyValid(const QString &deviceId, Metrics::MetricId metricId)
{
    return !deviceId.isEmpty() && metricId != Metrics::MetricId::Unknown;
}

QString DashboardMetricsModel::makeWidgetId(const QString &deviceId, Metrics::MetricId metricId)
{
    if (!isMetricKeyValid(deviceId, metricId))
        return {};

    return deviceId + QStringLiteral(":") + Metrics::metricIdToString(metricId);
}

bool DashboardMetricsModel::deviceIdContains(const MetricDescriptor &descriptor, const QString &text)
{
    if (text.isEmpty())
        return true;

    return descriptor.deviceId.contains(text, Qt::CaseInsensitive);
}

int DashboardMetricsModel::widgetIndexById(const QString &widgetId) const
{
    const auto indexIt = m_widgetIndexById.constFind(widgetId);
    return indexIt == m_widgetIndexById.constEnd() ? -1 : indexIt.value();
}

int DashboardMetricsModel::widgetIndexForMetric(const QString &deviceId, Metrics::MetricId metricId) const
{
    return widgetIndexById(makeWidgetId(deviceId, metricId));
}

bool DashboardMetricsModel::addWidget(const MetricDescriptor &descriptor, const QString &variant)
{
    const QString widgetId = makeWidgetId(descriptor.deviceId, descriptor.metricId);
    if (widgetId.isEmpty())
        return false;

    return insertWidget({
        widgetId,
        descriptor.deviceId,
        descriptor.displayName,
        0.0,
        variant.isEmpty() ? QStringLiteral("segments") : variant,
        descriptor.metricId,
        descriptor.unit.isEmpty() ? Metrics::metricUnit(descriptor.metricId) : descriptor.unit,
        descriptor.showProgressBar
    });
}

bool DashboardMetricsModel::addFirstDefaultWidget(Metrics::MetricId metricId,
                                                  const QString &deviceIdText,
                                                  const QString &variant)
{
    for (const MetricDescriptor &descriptor : m_availableMetrics) {
        if (descriptor.metricId == metricId && deviceIdContains(descriptor, deviceIdText))
            return addWidget(descriptor, variant);
    }

    return false;
}

bool DashboardMetricsModel::insertWidget(const WidgetItem &item)
{
    if (item.widgetId.isEmpty() || m_widgetIndexById.contains(item.widgetId))
        return false;

    const int insertRow = m_items.size();
    beginInsertRows(QModelIndex(), insertRow, insertRow);
    m_items.push_back(item);
    m_widgetIndexById.insert(item.widgetId, insertRow);
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
    m_widgetIndexById.clear();
    for (int i = 0; i < m_items.size(); ++i) {
        const WidgetItem &item = m_items.at(i);
        if (item.widgetId.isEmpty())
            continue;

        m_widgetIndexById.insert(item.widgetId, i);
    }
}

bool DashboardMetricsModel::setWidgetValue(const QString &deviceId,
                                           Metrics::MetricId metricId,
                                           double value,
                                           const QString &unit)
{
    const int index = widgetIndexForMetric(deviceId, metricId);
    if (index < 0)
        return false;

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
        return true;

    const QModelIndex modelIndex = this->index(index);
    emit dataChanged(modelIndex, modelIndex, changedRoles);
    return true;
}


void DashboardMetricsModel::syncInitialWidgetsWithMetrics()
{
    if (m_hasSeededInitialWidgets)
        return;

    addFirstDefaultWidget(Metrics::MetricId::Loading, QStringLiteral("cpu"), QStringLiteral("arc180"));
    addFirstDefaultWidget(Metrics::MetricId::Temperature, QStringLiteral("cpu"), QStringLiteral("segments"));
    addFirstDefaultWidget(Metrics::MetricId::Loading, QStringLiteral("gpu"), QStringLiteral("arc180"));
    addFirstDefaultWidget(Metrics::MetricId::Temperature, QStringLiteral("gpu"), QStringLiteral("segments"));
    addFirstDefaultWidget(Metrics::MetricId::Loading, QStringLiteral("ram"), QStringLiteral("segments"));
    addFirstDefaultWidget(Metrics::MetricId::BatteryLevel, QString(), QStringLiteral("segments"));

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
