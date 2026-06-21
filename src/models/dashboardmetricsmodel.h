#ifndef DASHBOARDMETRICSMODEL_H
#define DASHBOARDMETRICSMODEL_H

#include "../core/metricdescriptor.h"
#include "../core/displaymode.h"

#include <QAbstractListModel>
#include <QHash>
#include <QJsonArray>
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
        DisplayModeRole,
        MetricIdRole,
        UnitRole,
        ShowProgressBarRole,
        WidgetTypeRole,
        MetricValuesRole
    };
    Q_ENUM(Roles)

    explicit DashboardMetricsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QVariantMap get(int row) const;
    QJsonArray toJson() const;
    void restoreFromJson(const QJsonArray &widgets);
    bool hasSeededInitialWidgets() const;
    Q_INVOKABLE QVariantList availableDevices() const;
    Q_INVOKABLE QVariantList availableMetricsForDevice(const QString &deviceId) const;
    Q_INVOKABLE bool addWidgetForMetric(const QString &deviceId,
                                         const QString &metricId);
    Q_INVOKABLE bool removeWidget(const QString &widgetId);
    Q_INVOKABLE bool moveWidget(int from, int to);
    Q_INVOKABLE bool setDisplayMode(const QString &widgetId, DashboardDisplay::Mode displayMode);
    Q_INVOKABLE bool updateWidget(const QString &deviceId,
                                  Metrics::MetricId metricId,
                                  double value);

signals:
    void widgetsStateChanged();

public slots:
    void onAvailableMetricsChanged(const QList<MetricDescriptor> &metrics);
    void onMetricUpdated(const QString &deviceId,
                         Metrics::MetricId metricId,
                         double value);

private:
    enum class WidgetType {
        Metric,
        Network
    };

    struct WidgetItem {
        QString widgetId;
        QString deviceId;
        QString title;
        WidgetType type = WidgetType::Metric;
        DashboardDisplay::Mode displayMode = DashboardDisplay::Mode::Segments;
        Metrics::MetricId metricId = Metrics::MetricId::Unknown;
        QList<Metrics::MetricId> metricIds;
        QHash<Metrics::MetricId, double> metricValues;
        QString unit;
        bool showProgressBar = false;
    };

    static bool isMetricKeyValid(const QString &deviceId, Metrics::MetricId metricId);
    static QString makeWidgetId(const QString &deviceId, Metrics::MetricId metricId);
    static bool deviceIdContains(const MetricDescriptor &descriptor, const QString &text);
    int widgetIndexById(const QString &widgetId) const;
    int widgetIndexForMetric(const QString &deviceId, Metrics::MetricId metricId) const;
    bool addWidget(const MetricDescriptor &descriptor, DashboardDisplay::Mode displayMode);
    bool addFirstDefaultWidget(Metrics::MetricId metricId,
                               const QString &deviceIdText,
                               DashboardDisplay::Mode displayMode);
    bool insertWidget(const WidgetItem &item);
    bool removeWidgetAt(int index);
    void rebuildWidgetIndexes();
    bool setWidgetValue(const QString &deviceId,
                        Metrics::MetricId metricId,
                        double value,
                        const QString &unit = QString());
    static QString widgetTypeToString(WidgetType type);
    static QVariantMap metricValues(const WidgetItem &item);
    void syncInitialWidgetsWithMetrics();
    const MetricDescriptor *descriptorForMetric(const QString &deviceId,
                                                Metrics::MetricId metricId) const;

    QVector<WidgetItem> m_items;
    QList<MetricDescriptor> m_availableMetrics;
    QHash<QString, int> m_widgetIndexById;
    bool m_hasSeededInitialWidgets = false;
};

#endif // DASHBOARDMETRICSMODEL_H
