#ifndef DASHBOARDMETRICSMODEL_H
#define DASHBOARDMETRICSMODEL_H

#include "metricdescriptor.h"

#include <QAbstractListModel>
#include <QHash>
#include <QVector>
#include <QVariantList>

struct DashboardMetricWidgetKey
{
    QString title;
    Metrics::MetricId metricId = Metrics::MetricId::Unknown;

    bool operator==(const DashboardMetricWidgetKey &other) const
    {
        return title == other.title && metricId == other.metricId;
    }

    bool isValid() const
    {
        return !title.isEmpty() && metricId != Metrics::MetricId::Unknown;
    }
};

uint qHash(const DashboardMetricWidgetKey &key, uint seed = 0);

class DashboardMetricsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        WidgetIdRole = Qt::UserRole + 1,
        TitleRole,
        ValueRole,
        VariantRole,
        MetricIdRole,
        UnitRole
    };
    Q_ENUM(Roles)

    explicit DashboardMetricsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QVariantMap get(int row) const;
    Q_INVOKABLE QVariantList availableDevices() const;
    Q_INVOKABLE QVariantList availableMetricsForDevice(const QString &deviceId) const;
    Q_INVOKABLE bool addWidget(const QString &deviceId,
                               Metrics::MetricId metricId,
                               const QString &title,
                               const QString &unit,
                               const QString &variant);
    Q_INVOKABLE bool addWidgetForMetric(const QString &deviceId,
                                         const QString &metricId,
                                         const QString &variant = QStringLiteral("segments"));
    Q_INVOKABLE bool removeWidget(const QString &widgetId);
    Q_INVOKABLE bool moveWidget(int from, int to);
    Q_INVOKABLE bool setVariant(const QString &widgetId, const QString &variant);
    Q_INVOKABLE bool updateWidget(const QString &title,
                                  Metrics::MetricId metricId,
                                  int value);

public slots:
    void onAvailableMetricsChanged(const QList<MetricDescriptor> &metrics);
    void onMetricUpdated(const QString &title,
                         Metrics::MetricId metricId,
                         const QVariant &value);

private:
    struct WidgetItem {
        QString widgetId;
        QString deviceId;
        QString title;
        int value = 0;
        QString variant;
        Metrics::MetricId metricId = Metrics::MetricId::Unknown;
        QString unit;
    };

    static DashboardMetricWidgetKey makeWidgetKey(const QString &title, Metrics::MetricId metricId);
    static QString makeWidgetId(const DashboardMetricWidgetKey &key);
    int widgetIndexById(const QString &widgetId) const;
    int widgetIndexForMetric(const QString &title, Metrics::MetricId metricId) const;
    bool insertWidget(const WidgetItem &item);
    bool removeWidgetAt(int index);
    void rebuildWidgetIndexes();
    void setWidgetValue(const QString &title,
                        Metrics::MetricId metricId,
                        int value,
                        const QString &unit = QString());
    void syncInitialWidgetsWithMetrics();
    const MetricDescriptor *descriptorForMetric(const QString &deviceId,
                                                Metrics::MetricId metricId) const;

    QVector<WidgetItem> m_items;
    QList<MetricDescriptor> m_availableMetrics;
    QHash<DashboardMetricWidgetKey, int> m_widgetIndexByKey;
    QHash<DashboardMetricWidgetKey, QVariant> m_latestMetricValues;
    bool m_hasSeededInitialWidgets = false;
};

#endif // DASHBOARDMETRICSMODEL_H
