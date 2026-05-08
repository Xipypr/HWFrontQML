#ifndef DASHBOARDMETRICSMODEL_H
#define DASHBOARDMETRICSMODEL_H

#include "metricdescriptor.h"

#include <QAbstractListModel>
#include <QVector>
#include <QVariantList>


class DashboardMetricsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        WidgetIdRole = Qt::UserRole + 1,
        TitleRole,
        ValueRole,
        VariantRole,
        AvailableRole,
        MetricIdRole,
        UnitRole
    };
    Q_ENUM(Roles)

    enum WidgetType {
        Cpu = 0,
        Ram,
        Gpu,
        Hdd,
        Unknown
    };
    Q_ENUM(WidgetType)

    explicit DashboardMetricsModel(QObject *parent = nullptr);


    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QVariantMap get(int row) const;
    Q_INVOKABLE bool addWidget(const QString &title,
                               Metrics::MetricId metricId,
                               const QString &variant,
                               bool available = true,
                               const QString &unit = QString());
    Q_INVOKABLE bool addWidgetByType(WidgetType type);
    Q_INVOKABLE QVariantList widgetTypeOptions() const;
    Q_INVOKABLE bool removeWidget(const QString &widgetId);
    Q_INVOKABLE bool moveWidget(int from, int to);
    Q_INVOKABLE bool setVariant(const QString &widgetId, const QString &variant);
    Q_INVOKABLE bool updateWidget(const QString &title,
                                  Metrics::MetricId metricId,
                                  int value,
                                  bool available = true);

public slots:
    void onAvailableMetricsChanged(const QList<MetricDescriptor> &metrics);
    void onMetricUpdated(const QString &title,
                         Metrics::MetricId metricId,
                         const QVariant &value);

private:
    struct WidgetItem {
        QString widgetId;
        QString title;
        int value = 0;
        QString variant;
        bool available = true;
        Metrics::MetricId metricId = Metrics::MetricId::Unknown;
        QString unit;
    };

    struct WidgetDescriptor {
        WidgetType type = Unknown;
        QString title;
        QString variant;
    };

    static QString makeWidgetId(const QString &title, Metrics::MetricId metricId);
    int widgetIndexById(const QString &widgetId) const;
    int widgetIndexForMetric(const QString &title, Metrics::MetricId metricId) const;
    WidgetDescriptor descriptorForType(WidgetType type) const;
    void setWidgetValue(const QString &title,
                        Metrics::MetricId metricId,
                        int value,
                        bool available,
                        const QString &unit = QString());
    void setWidgetAvailability(const QString &title, Metrics::MetricId metricId, bool available);
    void syncWidgetsWithMetrics(const QList<MetricDescriptor> &metrics);

    QVector<WidgetItem> m_items;
};

#endif // DASHBOARDMETRICSMODEL_H
