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
    QList<Line> list;
    const QString fileName;
    QStandardItemModel *model;
    static void syncFileNames(ResultsData *rd1, ResultsData *rd2);
};

#endif // RESULTSDATA_H
