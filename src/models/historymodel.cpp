#include "historymodel.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>

HistoryModel::HistoryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    load();
}

int HistoryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_entries.size();
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_entries.size()) return QVariant();

    const auto &entry = m_entries[index.row()];
    switch (role) {
    case TitleRole: {
        if (!entry.title.isEmpty()) return entry.title;
        QFileInfo fi(entry.path);
        return fi.fileName();
    }
    case PathRole:
        return entry.path;
    case DurationRole:
        return entry.duration;
    case LastPositionRole:
        return entry.lastPosition;
    case LastPlayedRole:
        return entry.lastPlayed;
    case LastPlayedTextRole: {
        QDateTime now = QDateTime::currentDateTime();
        qint64 secsAgo = entry.lastPlayed.secsTo(now);
        if (secsAgo < 60) return tr("Just now");
        if (secsAgo < 3600) return tr("%1 min ago").arg(secsAgo / 60);
        if (secsAgo < 86400) return tr("%1 hours ago").arg(secsAgo / 3600);
        if (secsAgo < 604800) return tr("%1 days ago").arg(secsAgo / 86400);
        return entry.lastPlayed.toString("yyyy-MM-dd");
    }
    case ProgressRole:
        return entry.duration > 0 ? entry.lastPosition / entry.duration : 0.0;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> HistoryModel::roleNames() const
{
    return {
        {TitleRole, "title"},
        {PathRole, "path"},
        {DurationRole, "duration"},
        {LastPositionRole, "lastPosition"},
        {LastPlayedRole, "lastPlayed"},
        {LastPlayedTextRole, "lastPlayedText"},
        {ProgressRole, "progress"}
    };
}

void HistoryModel::addEntry(const QString &path, const QString &title,
                            double duration, double position)
{
    // Remove existing entry for this path
    for (int i = 0; i < m_entries.size(); ++i) {
        if (m_entries[i].path == path) {
            beginRemoveRows(QModelIndex(), i, i);
            m_entries.removeAt(i);
            endRemoveRows();
            break;
        }
    }

    // Insert at top
    Entry entry;
    entry.path = path;
    entry.title = title;
    entry.duration = duration;
    entry.lastPosition = position;
    entry.lastPlayed = QDateTime::currentDateTime();

    beginInsertRows(QModelIndex(), 0, 0);
    m_entries.prepend(entry);
    endInsertRows();

    // Trim to max
    while (m_entries.size() > MAX_ENTRIES) {
        beginRemoveRows(QModelIndex(), m_entries.size() - 1, m_entries.size() - 1);
        m_entries.removeLast();
        endRemoveRows();
    }

    emit countChanged();
    save();
}

void HistoryModel::removeEntry(int index)
{
    if (index < 0 || index >= m_entries.size()) return;
    beginRemoveRows(QModelIndex(), index, index);
    m_entries.removeAt(index);
    endRemoveRows();
    emit countChanged();
    save();
}

void HistoryModel::clearAll()
{
    beginResetModel();
    m_entries.clear();
    endResetModel();
    emit countChanged();
    save();
}

double HistoryModel::getResumePosition(const QString &path) const
{
    for (const auto &entry : m_entries) {
        if (entry.path == path) {
            // Only resume if not near the end (>95% watched)
            if (entry.duration > 0 && entry.lastPosition / entry.duration < 0.95) {
                return entry.lastPosition;
            }
            return 0;
        }
    }
    return 0;
}

QVariantList HistoryModel::recentFiles(int maxCount) const
{
    QVariantList result;
    int count = qMin(maxCount, m_entries.size());
    for (int i = 0; i < count; ++i) {
        QVariantMap item;
        item["title"] = m_entries[i].title.isEmpty()
            ? QFileInfo(m_entries[i].path).fileName()
            : m_entries[i].title;
        item["path"] = m_entries[i].path;
        result.append(item);
    }
    return result;
}

void HistoryModel::load()
{
    QFile file(historyFilePath());
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray arr = doc.array();

    beginResetModel();
    m_entries.clear();
    for (const auto &val : arr) {
        QJsonObject obj = val.toObject();
        Entry entry;
        entry.path = obj["path"].toString();
        entry.title = obj["title"].toString();
        entry.duration = obj["duration"].toDouble();
        entry.lastPosition = obj["position"].toDouble();
        entry.lastPlayed = QDateTime::fromString(obj["lastPlayed"].toString(), Qt::ISODate);
        if (entry.lastPlayed.isValid())
            m_entries.append(entry);
    }
    endResetModel();
}

void HistoryModel::save() const
{
    QJsonArray arr;
    for (const auto &entry : m_entries) {
        QJsonObject obj;
        obj["path"] = entry.path;
        obj["title"] = entry.title;
        obj["duration"] = entry.duration;
        obj["position"] = entry.lastPosition;
        obj["lastPlayed"] = entry.lastPlayed.toString(Qt::ISODate);
        arr.append(obj);
    }

    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);

    QFile file(historyFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(arr).toJson(QJsonDocument::Compact));
    }
}

QString HistoryModel::historyFilePath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
           + "/history.json";
}
