#include "trackmodel.h"
#include "core/mpvengine.h"

TrackModel::TrackModel(MpvEngine *engine, const QString &type, QObject *parent)
    : QAbstractListModel(parent)
    , m_engine(engine)
    , m_type(type)
{
    connect(m_engine, &MpvEngine::tracksChanged, this, &TrackModel::onTracksChanged);
}

int TrackModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_tracks.size();
}

QVariant TrackModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_tracks.size()) return QVariant();

    QVariantMap track = m_tracks[index.row()].toMap();
    switch (role) {
    case IdRole:
        return track["id"];
    case TitleRole:
        return track["title"].toString();
    case LangRole:
        return track["lang"].toString();
    case CodecRole:
        return track["codec"].toString();
    case IsSelectedRole:
        return track["selected"].toBool();
    case IsExternalRole:
        return track["external"].toBool();
    case DescriptionRole: {
        QStringList parts;
        int id = track["id"].toInt();
        parts << QString("#%1").arg(id);
        if (!track["title"].toString().isEmpty())
            parts << track["title"].toString();
        if (!track["lang"].toString().isEmpty())
            parts << QString("[%1]").arg(track["lang"].toString());
        if (!track["codec"].toString().isEmpty())
            parts << QString("(%1)").arg(track["codec"].toString());
        if (track["external"].toBool())
            parts << tr("[External]");
        return parts.join(" ");
    }
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> TrackModel::roleNames() const
{
    return {
        {IdRole, "trackId"},
        {TitleRole, "title"},
        {LangRole, "lang"},
        {CodecRole, "codec"},
        {IsSelectedRole, "isSelected"},
        {IsExternalRole, "isExternal"},
        {DescriptionRole, "description"}
    };
}

void TrackModel::onTracksChanged()
{
    QVariantList tracks;
    if (m_type == "sub") tracks = m_engine->subtitleTracks();
    else if (m_type == "audio") tracks = m_engine->audioTracks();
    else if (m_type == "video") tracks = m_engine->videoTracks();

    beginResetModel();
    m_tracks = tracks;
    endResetModel();
}
