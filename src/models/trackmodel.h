#ifndef TRACKMODEL_H
#define TRACKMODEL_H

#include <QAbstractListModel>
#include <QVariantList>

class MpvEngine;

class TrackModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString trackType READ trackType CONSTANT)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        LangRole,
        CodecRole,
        IsSelectedRole,
        IsExternalRole,
        DescriptionRole
    };

    explicit TrackModel(MpvEngine *engine, const QString &type, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString trackType() const { return m_type; }

private slots:
    void onTracksChanged();

private:
    MpvEngine *m_engine;
    QString m_type; // "sub", "audio", "video"
    QVariantList m_tracks;
};

#endif // TRACKMODEL_H
