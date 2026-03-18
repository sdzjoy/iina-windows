#include "playlistmodel.h"
#include "core/mpvengine.h"

#include <QFileInfo>
#include <QUrl>

PlaylistModel::PlaylistModel(MpvEngine *engine, QObject *parent)
    : QAbstractListModel(parent)
    , m_engine(engine)
{
    connect(m_engine, &MpvEngine::playlistChanged, this, &PlaylistModel::onPlaylistChanged);
    connect(m_engine, &MpvEngine::playlistPosChanged, this, &PlaylistModel::onPlaylistPosChanged);
    connect(m_engine, &MpvEngine::fileLoaded, this, &PlaylistModel::refresh);
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_items.size();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size()) return QVariant();

    QVariantMap item = m_items[index.row()].toMap();
    switch (role) {
    case TitleRole: {
        QString title = item["title"].toString();
        if (title.isEmpty()) {
            QString filename = item["filename"].toString();
            QFileInfo fi(filename);
            title = fi.fileName();
            if (title.isEmpty()) title = filename;
        }
        return title;
    }
    case PathRole:
        return item["filename"].toString();
    case IsCurrentRole:
        return item["current"].toBool();
    case IsPlayingRole:
        return item["playing"].toBool();
    case IndexRole:
        return index.row();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    return {
        {TitleRole, "title"},
        {PathRole, "path"},
        {IsCurrentRole, "isCurrent"},
        {IsPlayingRole, "isPlaying"},
        {IndexRole, "itemIndex"}
    };
}

void PlaylistModel::refresh()
{
    QVariant playlist = m_engine->getProperty("playlist");
    QVariantList items = playlist.toList();

    beginResetModel();
    m_items = items;
    endResetModel();

    int pos = m_engine->playlistPos();
    if (m_currentIndex != pos) {
        m_currentIndex = pos;
        emit currentIndexChanged();
    }
}

void PlaylistModel::onPlaylistChanged()
{
    refresh();
}

void PlaylistModel::onPlaylistPosChanged()
{
    int oldIndex = m_currentIndex;
    m_currentIndex = m_engine->playlistPos();

    if (oldIndex >= 0 && oldIndex < m_items.size()) {
        QModelIndex idx = index(oldIndex);
        emit dataChanged(idx, idx, {IsCurrentRole, IsPlayingRole});
    }
    if (m_currentIndex >= 0 && m_currentIndex < m_items.size()) {
        QModelIndex idx = index(m_currentIndex);
        emit dataChanged(idx, idx, {IsCurrentRole, IsPlayingRole});
    }
    emit currentIndexChanged();
}
