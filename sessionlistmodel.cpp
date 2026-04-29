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
    case AliasRole:
        return row.alias;
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
        { AliasRole, "alias" },
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
    Session *row = findMutableBySessionId(sessionId);
    if (!row) {
        return;
    }

    row->displayName = row->displayName.isEmpty() ? row->target : row->displayName;
    row->state = state;

    const int idx = indexOf(sessionId);
    emit dataChanged(index(idx, 0), index(idx, 0));
}


void SessionListModel::setSessionAlias(const QString &sessionId, const QString &alias)
{
    Session *row = findMutableBySessionId(sessionId);
    if (!row) {
        return;
    }

    if (row->alias == alias) {
        return;
    }

    row->alias = alias;

    const int idx = indexOf(sessionId);
    emit dataChanged(index(idx, 0), index(idx, 0));
}

QString SessionListModel::aliasForSession(const QString &sessionId) const
{
    const Session *row = findBySessionId(sessionId);
    if (!row) {
        return {};
    }

    return row->alias;
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

Session *SessionListModel::findMutableBySessionId(const QString &sessionId)
{
    const int idx = indexOf(sessionId);
    if (idx < 0) {
        return nullptr;
    }

    return &m_rows[idx];
}

const Session *SessionListModel::findBySessionId(const QString &sessionId) const
{
    const int idx = indexOf(sessionId);
    if (idx < 0) {
        return nullptr;
    }

    return &m_rows.at(idx);
}
