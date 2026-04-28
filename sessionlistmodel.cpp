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

    const SessionRow &row = m_rows.at(index.row());
    switch (role) {
    case SessionIdRole:
        return row.session.sessionId;
    case TargetRole:
        return row.session.target;
    case DisplayNameRole:
        return row.session.displayName;
    case StateRole:
        return SessionStateNs::toString(row.session.state);
    case HasDeviceRole:
        return row.hasDevice;
    case LastErrorRole:
        return row.lastError;
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
        { HasDeviceRole, "hasDevice" },
        { LastErrorRole, "lastError" }
    };
}

void SessionListModel::upsertSession(const Session &session, bool hasDevice, const QString &lastError)
{
    const int idx = indexOf(session.sessionId);
    if (idx >= 0) {
        SessionRow &row = m_rows[idx];
        row.session = session;
        row.hasDevice = hasDevice;
        row.lastError = lastError;
        emit dataChanged(index(idx, 0), index(idx, 0));
        return;
    }

    beginInsertRows(QModelIndex(), m_rows.size(), m_rows.size());
    m_rows.push_back({session, hasDevice, lastError});
    endInsertRows();
}

void SessionListModel::setSessionState(const QString &sessionId, const QString &state, const QString &lastError)
{
    const int idx = indexOf(sessionId);
    if (idx < 0) {
        return;
    }

    SessionRow &row = m_rows[idx];
    row.lastError = lastError;
    row.session.displayName = row.session.displayName.isEmpty() ? row.session.target : row.session.displayName;
    const QMetaEnum enumMeta = QMetaEnum::fromType<SessionState>();
    row.session.state = static_cast<SessionState>(enumMeta.keyToValue(state.toLatin1().constData()));
    emit dataChanged(index(idx, 0), index(idx, 0));
}

void SessionListModel::setDeviceReady(const QString &sessionId, const QString &displayName)
{
    const int idx = indexOf(sessionId);
    if (idx < 0) {
        return;
    }

    SessionRow &row = m_rows[idx];
    row.hasDevice = true;
    if (!displayName.isEmpty()) {
        row.session.displayName = displayName;
    }
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

int SessionListModel::indexOf(const QString &sessionId) const
{
    for (int i = 0; i < m_rows.size(); ++i) {
        if (m_rows.at(i).session.sessionId == sessionId) {
            return i;
        }
    }

    return -1;
}
