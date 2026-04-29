#include "sessionlistmodel.h"

SessionListModel::SessionListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int SessionListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_rows.size();
}

QVariant SessionListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_rows.size()) {
        return {};
    }

    const Session &row = m_rows.at(index.row());
    switch (role) {
    case SessionIdRole:
        return row.sessionId;
    case TargetRole:
        return row.target;
    case DisplayNameRole:
        return row.displayName;
    case StateRole:
        return SessionStateNs::toString(row.state);
    case HasDeviceRole:
        return row.hasDevice;
    default:
        return {};
    }
}

QHash<int, QByteArray> SessionListModel::roleNames() const
{
    return {
        { SessionIdRole, "sessionId" },
        { TargetRole, "target" },
        { DisplayNameRole, "displayName" },
        { StateRole, "state" },
        { HasDeviceRole, "hasDevice" }
    };
}

void SessionListModel::upsertSession(const Session &session)
{
    const int idx = indexOf(session.sessionId);
    if (idx >= 0) {
        m_rows[idx] = session;
        emit dataChanged(index(idx, 0), index(idx, 0));
        return;
    }

    beginInsertRows(QModelIndex(), m_rows.size(), m_rows.size());
    m_rows.push_back(session);
    endInsertRows();
}

void SessionListModel::setSessionState(const QString &sessionId, SessionState state)
{
    const int idx = indexOf(sessionId);
    if (idx < 0) {
        return;
    }

    Session &row = m_rows[idx];
    row.displayName = row.displayName.isEmpty() ? row.target : row.displayName;
    row.state = state;
    emit dataChanged(index(idx, 0), index(idx, 0));
}

void SessionListModel::removeSession(const QString &sessionId)
{
    const int idx = indexOf(sessionId);
    if (idx < 0) {
        return;
    }

    beginRemoveRows(QModelIndex(), idx, idx);
    m_rows.removeAt(idx);
    endRemoveRows();
}

QStringList SessionListModel::connectedSessionIds() const
{
    QStringList ids;
    ids.reserve(m_rows.size());

    for (const Session &row : m_rows) {
        if (row.hasDevice) {
            ids.push_back(row.sessionId);
        }
    }

    return ids;
}

int SessionListModel::indexOf(const QString &sessionId) const
{
    for (int i = 0; i < m_rows.size(); ++i) {
        if (m_rows.at(i).sessionId == sessionId) {
            return i;
        }
    }

    return -1;
}
