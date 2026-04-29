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
        AliasRole,
        StateRole,
        HasDeviceRole
    };

    explicit SessionListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void upsertSession(const Session &session);
    void setSessionState(const QString &sessionId, SessionState state);
    void removeSession(const QString &sessionId);
    void setSessionAlias(const QString &sessionId, const QString &alias);
    QString aliasForSession(const QString &sessionId) const;
    QStringList connectedSessionIds() const;

private:
    int indexOf(const QString &sessionId) const;
    Session *findMutableBySessionId(const QString &sessionId);
    const Session *findBySessionId(const QString &sessionId) const;

    QVector<Session> m_rows;
};

#endif // SESSIONLISTMODEL_H
