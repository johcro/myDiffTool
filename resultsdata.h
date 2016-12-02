#ifndef RESULTSDATA_H
#define RESULTSDATA_H

#include <QList>
#include <QString>

class QStandardItemModel;

class ResultsData
{
public:
    ResultsData(const QString &filename);
    ~ResultsData();

    bool load(const QString &fileName);
    void writeDataToModel();

    struct Line {
        QString sha;
        QString filename;
        QString line;
        QString column;
        QString severity;
        QString text;
        QString id;
        QString triage;
    };

    static QString getErrorGroup(QString id);
    static QStringList getErrorGroupList();

    static bool includeLineInExport(const Line &ln, const QString errorGroup);
    QList<Line> getResultsToExport(const QString errorGroup) const;
    static QList<Line> sort(QList<Line> results);

    void removeDuplicates();

    QList<Line> list;
    const QString fileName;
    QStandardItemModel *model;
    static void syncFileNames(ResultsData *rd1, ResultsData *rd2);

    QString report() const;

    static int getShaColumn() { return -1; }
    static int getFilenameColumn() { return 0; }
    static int getLineColumn() { return 1; }
    static int getSeverityColumn() { return -1; }
    static int getTextColumn() { return 2; }
    static int getIdColumn() { return 3; }
    static int getTriageColumn() { return 4; }
};

#endif // RESULTSDATA_H
