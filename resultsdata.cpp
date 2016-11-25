#include "resultsdata.h"
#include <QFile>
#include <QFileInfo>
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
    QRegExp rxLintWithSha("^([0-9a-f]{40}):(/repo/[^:]+):(\\d+):(.+)\\[([0-9]+)\\]$");
    QRegExp rxLintWithoutSha("^(/repo/[^:]+):(\\d+):(.+)\\[([0-9]+)\\]$");

    // Clang: 1234:/home/danielm/file1.c:14:12: warning: unused variable 'x' [-Wunused-variable]
    QRegExp rxClangWithSha("^([0-9a-f]{40}):(/repo/[^:]+):(\\d+):(\\d+):([ a-z]+):(.+)\\[-([0-9a-zA-Z\\-\\.]+)\\]$");
    QRegExp rxClangWithoutSha("^(/repo/[^:]+):(\\d+):(\\d+):([ a-z]+):(.+)\\[-?([0-9a-zA-Z\\-\\.]+)\\]$");

    QRegExp rxTriage("^(TP|FP|UNKNOWN).*");

    QTextStream in(&f);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        if (rxTriage.exactMatch(line)) {
            list.back().triage = line;
            continue;
        }
        if (rxLintWithSha.exactMatch(line)) {
            struct Line newLine;
            newLine.sha      = rxLintWithSha.cap(1);
            newLine.filename = rxLintWithSha.cap(2);
            newLine.line     = rxLintWithSha.cap(3);
            newLine.column.clear();
            newLine.severity.clear();
            newLine.text     = rxLintWithSha.cap(4);
            newLine.id       = rxLintWithSha.cap(5);
            list.append(newLine);
        } else if (rxLintWithoutSha.exactMatch(line)) {
            struct Line newLine;
            newLine.sha.clear();
            newLine.filename = rxLintWithoutSha.cap(1);
            newLine.line     = rxLintWithoutSha.cap(2);
            newLine.column.clear();
            newLine.severity.clear();
            newLine.text     = rxLintWithoutSha.cap(3);
            newLine.id       = rxLintWithoutSha.cap(4);
            list.append(newLine);
        } else if (rxClangWithSha.exactMatch(line)) {
            struct Line newLine;
            newLine.sha      = rxClangWithSha.cap(1);
            newLine.filename = rxClangWithSha.cap(2);
            newLine.line     = rxClangWithSha.cap(3);
            newLine.column   = rxClangWithSha.cap(4);
            newLine.severity = rxClangWithSha.cap(5);
            newLine.text     = rxClangWithSha.cap(6);
            newLine.id       = rxClangWithSha.cap(7);
            list.append(newLine);
        } else if (rxClangWithoutSha.exactMatch(line)) {
            struct Line newLine;
            newLine.sha.clear();
            newLine.filename = rxClangWithoutSha.cap(1);
            newLine.line     = rxClangWithoutSha.cap(2);
            newLine.column   = rxClangWithoutSha.cap(3);
            newLine.severity = rxClangWithoutSha.cap(4);
            newLine.text     = rxClangWithoutSha.cap(5);
            newLine.id       = rxClangWithoutSha.cap(6);
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

    model->setHeaderData(0, Qt::Horizontal, "File name");
    model->setHeaderData(1, Qt::Horizontal, "Line");
    model->setHeaderData(2, Qt::Horizontal, "Text");
    model->setHeaderData(3, Qt::Horizontal, "ID");
    model->setHeaderData(4, Qt::Horizontal, "Triage");

    for ( int row = 0; row < list.size(); ++row )
    {
        QModelIndex index;
        int col = 0;
        //index = model->index(row, col++, QModelIndex());
        //model->setData(index, list[row].sha);
        index = model->index(row, col++, QModelIndex());
        model->setData(index, QFileInfo(list[row].filename).fileName());
        index = model->index(row, col++, QModelIndex());
        model->setData(index, list[row].line);
        index = model->index(row, col++, QModelIndex());
        model->setData(index, list[row].text);
        index = model->index(row, col++, QModelIndex());
        model->setData(index, list[row].id);
        index = model->index(row, col++, QModelIndex());
        model->setData(index, list[row].triage);
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
