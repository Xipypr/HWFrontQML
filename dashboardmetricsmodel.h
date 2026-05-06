#ifndef DASHBOARDMETRICSMODEL_H
#define DASHBOARDMETRICSMODEL_H

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
        AvailableRole
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
    Q_INVOKABLE bool addWidget(const QString &widgetId,
                               const QString &variant,
                               bool available = true);
    Q_INVOKABLE bool addWidgetByType(WidgetType type);
    Q_INVOKABLE QVariantList widgetTypeOptions() const;
    Q_INVOKABLE bool removeWidget(const QString &widgetId);
    Q_INVOKABLE bool moveWidget(int from, int to);
    Q_INVOKABLE bool setVariant(const QString &widgetId, const QString &variant);
    Q_INVOKABLE bool updateWidget(const QString &widgetId,
                                  int value,
                                  bool available = true);

public slots:
    void onAvailableMetricsChanged(const QVariantList &metrics);
    void onMetricUpdated(const QString &deviceId,
                         const QString &metricId,
                         const QVariant &value);

private:
    struct WidgetItem {
        QString widgetId;
        QString title;
        int value = 0;
        QString variant;
        bool available = true;
    };

    struct WidgetDescriptor {
        WidgetType type = Unknown;
        QString widgetId;
        QString title;
        QString variant;
    };

    int findWidgetIndex(const QString &widgetId) const;
    WidgetDescriptor descriptorForType(WidgetType type) const;
    void setWidgetValue(const QString &widgetId, int value, bool available, const QString &title = QString());
    void syncWidgetsWithMetrics(const QVariantList &metrics);

    QVector<WidgetItem> m_items;
};

#endif // DASHBOARDMETRICSMODEL_H
