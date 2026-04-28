#ifndef SESSIONLISTMODEL_H
#define SESSIONLISTMODEL_H

#include "session.h"

#include <QAbstractListModel>
#include <QHash>
#include <QVector>

class SessionListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        SessionIdRole = Qt::UserRole + 1,
        TargetRole,
        DisplayNameRole,
        StateRole,
        HasDeviceRole,
        LastErrorRole
    };

    explicit SessionListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void upsertSession(const Session &session, bool hasDevice, const QString &lastError);
    void setSessionState(const QString &sessionId, const QString &state, const QString &lastError = QString());
    void setDeviceReady(const QString &sessionId, const QString &displayName);
    void removeSession(const QString &sessionId);

private:
    struct SessionRow {
        Session session;
        bool hasDevice = false;
        QString lastError;
    };

    int indexOf(const QString &sessionId) const;

    QVector<SessionRow> m_rows;
};

#endif // SESSIONLISTMODEL_H
