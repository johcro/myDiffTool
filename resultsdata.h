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
        QString text;
        QString id;
    };
    QList<Line> list;
    const QString fileName;
    QStandardItemModel *model;
};

#endif // RESULTSDATA_H
