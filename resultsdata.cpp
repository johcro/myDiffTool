#include "resultsdata.h"
#include <QFile>
#include <QRegExp>
#include <QStandardItemModel>
#include <QTextStream>

ResultsData::ResultsData(const QString &fileName)
    : fileName(fileName), model(0)
{
    load(fileName);
}

ResultsData::~ResultsData() {
    delete model;
}

bool ResultsData::load(const QString &fileName) {
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly))
        return false;

    // Lint: 1234:/home/danielm/file1.c:237: Signed-unsigned mix with relational [574]
    QRegExp rx1("^([0-9a-f]{40}):([^:]+):(\\d+):(.+)\\[([0-9]+)\\]$");

    // Clang: 1234:/home/danielm/file1.c:14:12: warning: unused variable 'x' [-Wunused-variable]
    QRegExp rx2("^([0-9a-f]{40}):([^:]+):(\\d+):(\\d+):([ a-z]+):(.+)\\[-([0-9a-zA-Z\\-\\.]+)\\]$");

    QTextStream in(&f);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        if (rx1.exactMatch(line)) {
            struct Line newLine;
            newLine.sha      = rx1.cap(1);
            newLine.filename = rx1.cap(2);
            newLine.line     = rx1.cap(3);
            newLine.column.clear();
            newLine.severity.clear();
            newLine.text     = rx1.cap(4);
            newLine.id       = rx1.cap(5);
            list.append(newLine);
        } else if (rx2.exactMatch(line)) {
            struct Line newLine;
            newLine.sha      = rx2.cap(1);
            newLine.filename = rx2.cap(2);
            newLine.line     = rx2.cap(3);
            newLine.column   = rx2.cap(4);
            newLine.severity = rx2.cap(5);
            newLine.text     = rx2.cap(6);
            newLine.id       = rx2.cap(7);
            list.append(newLine);
        }
    }
    writeDataToModel();
    return true;
}

void ResultsData::writeDataToModel()
{
    delete model;
    model = new QStandardItemModel(list.size(), 5);

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
}

void ResultsData::syncFileNames(ResultsData *rd1, ResultsData *rd2)
{
    Line emptyLine;
    for (int i = 0; i < rd1->list.size() && i < rd2->list.size(); ++i) {
        const QString &f1 = rd1->list[i].filename;
        const QString &f2 = rd2->list[i].filename;
        if (f1 > f2)
            rd1->list.insert(i, emptyLine);
        else if (f1 < f2)
            rd2->list.insert(i,emptyLine);
    }
    while (rd1->list.size() < rd2->list.size())
        rd1->list.append(emptyLine);
    while (rd1->list.size() > rd2->list.size())
        rd2->list.append(emptyLine);
    rd1->writeDataToModel();
    rd2->writeDataToModel();
}
