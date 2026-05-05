#ifndef DASHBOARDMETRICSMODEL_H
#define DASHBOARDMETRICSMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QVariantList>

class SessionManager;
class DesktopDevice;
class Device;

class DashboardMetricsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString sessionId READ sessionId WRITE setSessionId NOTIFY sessionIdChanged)

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

    QString sessionId() const;
    void setSessionId(const QString &sessionId);

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

signals:
    void sessionIdChanged();

private slots:
    void onDeviceReady(const QString &sessionId, DesktopDevice *deviceRef);

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
    void setWidgetValue(const QString &widgetId, int value, bool available);
    void applyDeviceSnapshot(const QList<Device *> &devices);

    QVector<WidgetItem> m_items;
    SessionManager *m_sessionManager = nullptr;
    QString m_sessionId;
};

#endif // DASHBOARDMETRICSMODEL_H
