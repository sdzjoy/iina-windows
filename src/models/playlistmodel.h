#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include <QVariantList>

class MpvEngine;

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged)

public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        PathRole,
        IsCurrentRole,
        IsPlayingRole,
        IndexRole
    };

    explicit PlaylistModel(MpvEngine *engine, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int currentIndex() const { return m_currentIndex; }

    Q_INVOKABLE void refresh();

signals:
    void currentIndexChanged();

private slots:
    void onPlaylistChanged();
    void onPlaylistPosChanged();

private:
    MpvEngine *m_engine;
    QVariantList m_items;
    int m_currentIndex = -1;
};

#endif // PLAYLISTMODEL_H
