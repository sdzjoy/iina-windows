#ifndef HISTORYMODEL_H
#define HISTORYMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QJsonArray>

class HistoryModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        PathRole,
        DurationRole,
        LastPositionRole,
        LastPlayedRole,
        LastPlayedTextRole,
        ProgressRole
    };

    struct Entry {
        QString title;
        QString path;
        double duration = 0;
        double lastPosition = 0;
        QDateTime lastPlayed;
    };

    explicit HistoryModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const { return m_entries.size(); }

    Q_INVOKABLE void addEntry(const QString &path, const QString &title,
                              double duration, double position);
    Q_INVOKABLE void removeEntry(int index);
    Q_INVOKABLE void clearAll();
    Q_INVOKABLE double getResumePosition(const QString &path) const;
    Q_INVOKABLE QVariantList recentFiles(int maxCount = 10) const;

signals:
    void countChanged();

private:
    void load();
    void save() const;
    QString historyFilePath() const;

    QVector<Entry> m_entries;
    static const int MAX_ENTRIES = 200;
};

#endif // HISTORYMODEL_H
