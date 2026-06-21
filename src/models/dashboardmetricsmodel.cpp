#include "dashboardmetricsmodel.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QSet>

DashboardMetricsModel::DashboardMetricsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    qRegisterMetaType<DashboardDisplay::Mode>("DashboardDisplay::Mode");
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
        return item.metricValues.value(item.metricId, 0.0);
    case DisplayModeRole:
        return QVariant::fromValue(item.displayMode);
    case MetricIdRole:
        return Metrics::metricIdToString(item.metricId);
    case UnitRole:
        return item.unit;
    case ShowProgressBarRole:
        return item.showProgressBar;
    case WidgetTypeRole:
        return widgetTypeToString(item.type);
    case MetricValuesRole:
        return metricValues(item);
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
        { DisplayModeRole, "displayMode" },
        { MetricIdRole, "metricId" },
        { UnitRole, "unit" },
        { ShowProgressBarRole, "showProgressBar" },
        { WidgetTypeRole, "widgetType" },
        { MetricValuesRole, "metricValues" }
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
        { "value", item.metricValues.value(item.metricId, 0.0) },
        { "displayMode", QVariant::fromValue(item.displayMode) },
        { "metricId", Metrics::metricIdToString(item.metricId) },
        { "widgetType", widgetTypeToString(item.type) },
        { "metricValues", metricValues(item) },
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
        widgetObject[QStringLiteral("widgetType")] = widgetTypeToString(item.type);
        widgetObject[QStringLiteral("metricId")] = Metrics::metricIdToString(item.metricId);
        QJsonArray metricIds;
        for (const Metrics::MetricId metricId : item.metricIds)
            metricIds.append(Metrics::metricIdToString(metricId));
        widgetObject[QStringLiteral("metricIds")] = metricIds;
        widgetObject[QStringLiteral("unit")] = item.unit;
        widgetObject[QStringLiteral("showProgressBar")] = item.showProgressBar;
        widgetObject[QStringLiteral("displayMode")] = static_cast<int>(item.displayMode);
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
        Metrics::MetricId metricId = Metrics::metricIdFromString(widgetObject.value(QStringLiteral("metricId")).toString());
        if (metricId == Metrics::MetricId::NetworkUpload)
            metricId = Metrics::MetricId::NetworkDownload;
        const QString widgetId = makeWidgetId(deviceId, metricId);

        if (widgetId.isEmpty() || restoredIndexes.contains(widgetId))
            continue;

        const bool networkWidget = widgetObject.value(QStringLiteral("widgetType")).toString() == QStringLiteral("network")
                || metricId == Metrics::MetricId::NetworkDownload;

        WidgetItem item;
        item.widgetId = widgetId;
        item.deviceId = deviceId;
        item.title = title;
        item.type = networkWidget ? WidgetType::Network : WidgetType::Metric;
        item.displayMode = static_cast<DashboardDisplay::Mode>(
            widgetObject.value(QStringLiteral("displayMode")).toInt());
        if (networkWidget && !DashboardDisplay::isNetworkMode(item.displayMode))
            item.displayMode = DashboardDisplay::Mode::NetworkVertical;
        if (!networkWidget && DashboardDisplay::isNetworkMode(item.displayMode))
            item.displayMode = DashboardDisplay::Mode::Segments;
        item.metricId = metricId;
        const QJsonArray savedMetricIds = widgetObject.value(QStringLiteral("metricIds")).toArray();
        for (const QJsonValue &savedMetricId : savedMetricIds) {
            const Metrics::MetricId parsedMetricId = Metrics::metricIdFromString(savedMetricId.toString());
            if (parsedMetricId != Metrics::MetricId::Unknown && !item.metricIds.contains(parsedMetricId))
                item.metricIds.append(parsedMetricId);
        }
        if (networkWidget) {
            item.metricIds = { Metrics::MetricId::NetworkDownload, Metrics::MetricId::NetworkUpload };
        } else if (item.metricIds.isEmpty()) {
            item.metricIds = { metricId };
        }
        for (const Metrics::MetricId itemMetricId : item.metricIds)
            item.metricValues.insert(itemMetricId, 0.0);
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
        if (descriptor.deviceId != deviceId
                || descriptor.metricId == Metrics::MetricId::Unknown
                || descriptor.metricId == Metrics::MetricId::NetworkUpload) {
            continue;
        }

        const QString metricName = Metrics::metricIdToString(descriptor.metricId);
        metrics.push_back(QVariantMap{
            { "metricId", metricName },
            { "label", descriptor.metricId == Metrics::MetricId::NetworkDownload
                           ? QStringLiteral("Network")
                           : metricName },
            { "title", descriptor.displayName },
            { "unit", descriptor.unit },
            { "showProgressBar", descriptor.showProgressBar }
        });
    }

    return metrics;
}

bool DashboardMetricsModel::addWidgetForMetric(const QString &deviceId,
                                               const QString &metricId)
{
    const Metrics::MetricId parsedMetricId = Metrics::metricIdFromString(metricId);
    if (deviceId.isEmpty() || parsedMetricId == Metrics::MetricId::Unknown)
        return false;

    const MetricDescriptor *descriptor = descriptorForMetric(deviceId, parsedMetricId);
    if (!descriptor)
        return false;

    return addWidget(*descriptor, defaultDisplayMode(parsedMetricId));
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

bool DashboardMetricsModel::setDisplayMode(const QString &widgetId,
                                           DashboardDisplay::Mode displayMode)
{
    const int index = widgetIndexById(widgetId);
    if (index < 0)
        return false;

    WidgetItem &item = m_items[index];
    if (item.type == WidgetType::Network && !DashboardDisplay::isNetworkMode(displayMode))
        return false;
    if (item.type == WidgetType::Metric && DashboardDisplay::isNetworkMode(displayMode))
        return false;

    if (item.displayMode == displayMode)
        return true;

    item.displayMode = displayMode;
    const QModelIndex modelIndex = this->index(index);
    emit dataChanged(modelIndex, modelIndex, { DisplayModeRole });
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
                                            double value)
{
    if (deviceId.isEmpty() || metricId == Metrics::MetricId::Unknown)
        return;

    setWidgetValue(deviceId, metricId, value);
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

DashboardDisplay::Mode DashboardMetricsModel::defaultDisplayMode(Metrics::MetricId metricId)
{
    if (metricId == Metrics::MetricId::NetworkDownload)
        return DashboardDisplay::Mode::NetworkVertical;

    return DashboardDisplay::Mode::Segments;
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

bool DashboardMetricsModel::addWidget(const MetricDescriptor &descriptor,
                                      DashboardDisplay::Mode displayMode)
{
    const QString widgetId = makeWidgetId(descriptor.deviceId, descriptor.metricId);
    if (widgetId.isEmpty())
        return false;

    WidgetItem item;
    item.widgetId = widgetId;
    item.deviceId = descriptor.deviceId;
    item.title = descriptor.displayName;
    item.metricId = descriptor.metricId;
    item.unit = descriptor.unit.isEmpty() ? Metrics::metricUnit(descriptor.metricId) : descriptor.unit;
    item.showProgressBar = descriptor.showProgressBar;
    item.displayMode = displayMode;

    if (descriptor.metricId == Metrics::MetricId::NetworkDownload) {
        item.type = WidgetType::Network;
        if (!DashboardDisplay::isNetworkMode(item.displayMode))
            item.displayMode = DashboardDisplay::Mode::NetworkVertical;
        item.metricIds = { Metrics::MetricId::NetworkDownload, Metrics::MetricId::NetworkUpload };
    } else {
        item.type = WidgetType::Metric;
        if (DashboardDisplay::isNetworkMode(item.displayMode))
            item.displayMode = DashboardDisplay::Mode::Segments;
        item.metricIds = { descriptor.metricId };
    }

    for (const Metrics::MetricId metricId : item.metricIds)
        item.metricValues.insert(metricId, 0.0);

    return insertWidget(item);
}

bool DashboardMetricsModel::addFirstDefaultWidget(Metrics::MetricId metricId,
                                                  const QString &deviceIdText,
                                                  DashboardDisplay::Mode displayMode)
{
    for (const MetricDescriptor &descriptor : m_availableMetrics) {
        if (descriptor.metricId == metricId && deviceIdContains(descriptor, deviceIdText))
            return addWidget(descriptor, displayMode);
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
    bool updated = false;

    for (int index = 0; index < m_items.size(); ++index) {
        WidgetItem &item = m_items[index];
        if (item.deviceId != deviceId || !item.metricIds.contains(metricId))
            continue;

        QVector<int> changedRoles;
        if (item.metricValues.value(metricId, 0.0) != value) {
            item.metricValues.insert(metricId, value);
            changedRoles.push_back(MetricValuesRole);
            if (item.metricId == metricId)
                changedRoles.push_back(ValueRole);
        }

        if (!unit.isEmpty() && item.unit != unit) {
            item.unit = unit;
            changedRoles.push_back(UnitRole);
        }

        if (!changedRoles.isEmpty()) {
            const QModelIndex modelIndex = this->index(index);
            emit dataChanged(modelIndex, modelIndex, changedRoles);
        }
        updated = true;
    }

    return updated;
}

QString DashboardMetricsModel::widgetTypeToString(WidgetType type)
{
    switch (type) {
    case WidgetType::Metric:
        return QStringLiteral("metric");
    case WidgetType::Network:
        return QStringLiteral("network");
    }

    return QStringLiteral("metric");
}

QVariantMap DashboardMetricsModel::metricValues(const WidgetItem &item)
{
    QVariantMap values;
    for (auto it = item.metricValues.cbegin(); it != item.metricValues.cend(); ++it)
        values.insert(Metrics::metricIdToString(it.key()), it.value());
    return values;
}


void DashboardMetricsModel::syncInitialWidgetsWithMetrics()
{
    if (m_hasSeededInitialWidgets)
        return;

    addFirstDefaultWidget(Metrics::MetricId::Loading,
                          QStringLiteral("cpu"),
                          DashboardDisplay::Mode::Arc180);
    addFirstDefaultWidget(Metrics::MetricId::Temperature,
                          QStringLiteral("cpu"),
                          DashboardDisplay::Mode::Segments);
    addFirstDefaultWidget(Metrics::MetricId::Loading,
                          QStringLiteral("gpu"),
                          DashboardDisplay::Mode::Arc180);
    addFirstDefaultWidget(Metrics::MetricId::Temperature,
                          QStringLiteral("gpu"),
                          DashboardDisplay::Mode::Segments);
    addFirstDefaultWidget(Metrics::MetricId::Loading,
                          QStringLiteral("ram"),
                          DashboardDisplay::Mode::Segments);
    addFirstDefaultWidget(Metrics::MetricId::BatteryLevel,
                          QString(),
                          DashboardDisplay::Mode::Segments);
    addFirstDefaultWidget(Metrics::MetricId::NetworkDownload,
                          QStringLiteral("nic"),
                          DashboardDisplay::Mode::NetworkVertical);

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
