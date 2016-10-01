#include "resultsdata.h"
#include <QFile>
#include <QRegExp>
#include <QStandardItemModel>
#include <QTextStream>

ResultsData::ResultsData(const QString &fileName)
    : fileName(fileName)
{
    load(fileName);
    model = new QStandardItemModel(list.size(), 5);
    writeDataToModel();
}

ResultsData::~ResultsData() {
    delete model;
}

bool ResultsData::load(const QString &fileName) {
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly))
        return false;

    QRegExp rx1("^([0-9a-f]{40}):([^:]+):(\\d+):([ a-z]+):(.+)\\[([0-9a-fA-F\\-\\.]+)\\]$");
    QRegExp rx2("^([0-9a-f]{40}):([^:]+):(\\d+):(\\d+):([ a-z]+):(.+)\\[([0-9a-fA-F\\-\\.]+)\\]$");
    QTextStream in(&f);
    while (!in.atEnd()) {
        const QString str = in.readLine();
        if (rx1.exactMatch(str)) {
            struct Line newLine;
            newLine.sha      = rx1.cap(0);
            newLine.filename = rx1.cap(1);
            newLine.line     = rx1.cap(2);
            newLine.column.clear();
            newLine.text     = rx1.cap(3);
            newLine.id       = rx1.cap(4);
            list.append(newLine);
        } else if (rx2.exactMatch(str)) {
            struct Line newLine;
            newLine.sha      = rx2.cap(0);
            newLine.filename = rx2.cap(1);
            newLine.line     = rx2.cap(2);
            newLine.column   = rx2.cap(3);
            newLine.text     = rx2.cap(4);
            newLine.id       = rx2.cap(5);
            list.append(newLine);
        }
    }
    return true;
}

void ResultsData::writeDataToModel()
{

    for ( int row = 0; row < list.size(); ++row )
    {
        QModelIndex index;
        index = model->index(row, 0, QModelIndex());
        model->setData(index, list[row].sha);
        index = model->index(row, 1, QModelIndex());
        model->setData(index, list[row].filename);
        index = model->index(row, 2, QModelIndex());
        model->setData(index, list[row].line);
        index = model->index(row, 3, QModelIndex());
        model->setData(index, list[row].text);
        index = model->index(row, 4, QModelIndex());
        model->setData(index, list[row].id);
    }
    model->sort(1);
}
