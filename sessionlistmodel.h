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
        StateRole
    };

    explicit SessionListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void upsertSession(const Session &session);
    void setSessionState(const QString &sessionId, const QString &state);
    void removeSession(const QString &sessionId);

private:
    struct SessionRow {
        Session session;
    };

    int indexOf(const QString &sessionId) const;

    QVector<SessionRow> m_rows;
};

#endif // SESSIONLISTMODEL_H
