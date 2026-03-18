#ifndef CHAPTERMODEL_H
#define CHAPTERMODEL_H

#include <QAbstractListModel>
#include <QVariantList>

class MpvEngine;

class ChapterModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY chaptersChanged)
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged)

public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        TimeRole,
        TimeTextRole,
        IndexRole,
        IsCurrentRole
    };

    explicit ChapterModel(MpvEngine *engine, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const { return m_chapters.size(); }
    int currentIndex() const { return m_currentIndex; }

    Q_INVOKABLE void seekToChapter(int index);
    Q_INVOKABLE void nextChapter();
    Q_INVOKABLE void prevChapter();

signals:
    void chaptersChanged();
    void currentIndexChanged();

private slots:
    void onFileLoaded();
    void onPositionChanged();

private:
    void refresh();
    static QString formatTime(double seconds);

    MpvEngine *m_engine;

    struct Chapter {
        QString title;
        double time;
    };
    QVector<Chapter> m_chapters;
    int m_currentIndex = -1;
};

#endif // CHAPTERMODEL_H
