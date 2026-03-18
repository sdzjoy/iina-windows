#include "chaptermodel.h"
#include "core/mpvengine.h"

#include <cmath>

ChapterModel::ChapterModel(MpvEngine *engine, QObject *parent)
    : QAbstractListModel(parent)
    , m_engine(engine)
{
    connect(m_engine, &MpvEngine::fileLoaded, this, &ChapterModel::onFileLoaded);
    connect(m_engine, &MpvEngine::positionChanged, this, &ChapterModel::onPositionChanged);
}

int ChapterModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_chapters.size();
}

QVariant ChapterModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_chapters.size()) return QVariant();

    const auto &ch = m_chapters[index.row()];
    switch (role) {
    case TitleRole: {
        QString title = ch.title;
        if (title.isEmpty())
            title = tr("Chapter %1").arg(index.row() + 1);
        return title;
    }
    case TimeRole:
        return ch.time;
    case TimeTextRole:
        return formatTime(ch.time);
    case IndexRole:
        return index.row();
    case IsCurrentRole:
        return index.row() == m_currentIndex;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ChapterModel::roleNames() const
{
    return {
        {TitleRole, "title"},
        {TimeRole, "time"},
        {TimeTextRole, "timeText"},
        {IndexRole, "chapterIndex"},
        {IsCurrentRole, "isCurrent"}
    };
}

void ChapterModel::seekToChapter(int index)
{
    if (index >= 0 && index < m_chapters.size()) {
        m_engine->seekAbsolute(m_chapters[index].time);
    }
}

void ChapterModel::nextChapter()
{
    if (m_currentIndex + 1 < m_chapters.size()) {
        seekToChapter(m_currentIndex + 1);
    }
}

void ChapterModel::prevChapter()
{
    // If more than 2 seconds into current chapter, seek to its start
    if (m_currentIndex >= 0 && m_currentIndex < m_chapters.size()) {
        double chStart = m_chapters[m_currentIndex].time;
        if (m_engine->position() - chStart > 2.0) {
            m_engine->seekAbsolute(chStart);
            return;
        }
    }
    if (m_currentIndex > 0) {
        seekToChapter(m_currentIndex - 1);
    }
}

void ChapterModel::onFileLoaded()
{
    refresh();
}

void ChapterModel::onPositionChanged()
{
    if (m_chapters.isEmpty()) return;

    double pos = m_engine->position();
    int newIndex = -1;
    for (int i = m_chapters.size() - 1; i >= 0; --i) {
        if (pos >= m_chapters[i].time) {
            newIndex = i;
            break;
        }
    }

    if (newIndex != m_currentIndex) {
        int old = m_currentIndex;
        m_currentIndex = newIndex;
        if (old >= 0 && old < m_chapters.size()) {
            QModelIndex idx = index(old);
            emit dataChanged(idx, idx, {IsCurrentRole});
        }
        if (m_currentIndex >= 0 && m_currentIndex < m_chapters.size()) {
            QModelIndex idx = index(m_currentIndex);
            emit dataChanged(idx, idx, {IsCurrentRole});
        }
        emit currentIndexChanged();
    }
}

void ChapterModel::refresh()
{
    beginResetModel();
    m_chapters.clear();
    m_currentIndex = -1;

    QVariant chapterList = m_engine->getProperty("chapter-list");
    QVariantList chapters = chapterList.toList();

    for (const auto &ch : chapters) {
        QVariantMap map = ch.toMap();
        Chapter chapter;
        chapter.title = map["title"].toString();
        chapter.time = map["time"].toDouble();
        m_chapters.append(chapter);
    }

    endResetModel();
    emit chaptersChanged();

    // Set initial index
    onPositionChanged();
}

QString ChapterModel::formatTime(double seconds)
{
    if (std::isnan(seconds) || seconds < 0) seconds = 0;
    int totalSecs = static_cast<int>(seconds);
    int hours = totalSecs / 3600;
    int mins = (totalSecs % 3600) / 60;
    int secs = totalSecs % 60;

    if (hours > 0)
        return QString::asprintf("%d:%02d:%02d", hours, mins, secs);
    else
        return QString::asprintf("%02d:%02d", mins, secs);
}
