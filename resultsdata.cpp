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

ResultsData::~ResultsData()
{
    delete model;
}

bool ResultsData::load(const QString &fileName)
{
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly))
        return false;

    // Lint: 1234:/home/danielm/file1.c:237: Signed-unsigned mix with relational [574]
    QRegExp rxLintWithSha("^([0-9a-f]{40}):(/repo/[^:]+):(\\d+):(.+)\\[([0-9]+)\\]$");
    QRegExp rxLintWithoutSha("^(/repo/[^:]+):(\\d+):(.+)\\[([0-9]+)\\]$");

    // Clang: 1234:/home/danielm/file1.c:14:12: warning: unused variable 'x' [-Wunused-variable]
    QRegExp rxClangWithSha("^([0-9a-f]{40}):(/repo/[^:]+):(\\d+):(\\d+):([ a-z]+):(.+)\\[-?([0-9a-zA-Z\\-\\.]+)\\]$");
    QRegExp rxClangWithoutSha("^(/repo/[^:]+):(\\d+):(\\d+):([ a-z]+):(.+)\\[-?([0-9a-zA-Z\\-\\.]+)\\]$");

    QRegExp rxTriage("^(TP|FP|UNKNOWN|DUPLICATE).*");

    int linenr = 0;
    int triage = 0;

    QTextStream in(&f);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        linenr++;
        if (linenr == triage && rxTriage.exactMatch(line)) {
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
            newLine.text     = rxLintWithSha.cap(4).trimmed();
            newLine.id       = rxLintWithSha.cap(5);
            list.append(newLine);
            triage = linenr + 1;
        } else if (rxLintWithoutSha.exactMatch(line)) {
            struct Line newLine;
            newLine.sha.clear();
            newLine.filename = rxLintWithoutSha.cap(1);
            newLine.line     = rxLintWithoutSha.cap(2);
            newLine.column.clear();
            newLine.severity.clear();
            newLine.text     = rxLintWithoutSha.cap(3).trimmed();
            newLine.id       = rxLintWithoutSha.cap(4);
            list.append(newLine);
            triage = linenr + 1;
        } else if (rxClangWithSha.exactMatch(line)) {
            struct Line newLine;
            newLine.sha      = rxClangWithSha.cap(1);
            newLine.filename = rxClangWithSha.cap(2);
            newLine.line     = rxClangWithSha.cap(3);
            newLine.column   = rxClangWithSha.cap(4);
            newLine.severity = rxClangWithSha.cap(5).trimmed();
            newLine.text     = rxClangWithSha.cap(6).trimmed();
            newLine.id       = rxClangWithSha.cap(7);
            list.append(newLine);
            triage = linenr + 1;
        } else if (rxClangWithoutSha.exactMatch(line)) {
            struct Line newLine;
            newLine.sha.clear();
            newLine.filename = rxClangWithoutSha.cap(1);
            newLine.line     = rxClangWithoutSha.cap(2);
            newLine.column   = rxClangWithoutSha.cap(3);
            newLine.severity = rxClangWithoutSha.cap(4).trimmed();
            newLine.text     = rxClangWithoutSha.cap(5).trimmed();
            newLine.id       = rxClangWithoutSha.cap(6);
            list.append(newLine);
            triage = linenr + 1;
        }
    }

    // Cleanup results..
    for (int i = 0; i < list.size(); ++i) {
        // Remove results that don't have a error group..
        if (ResultsData::getErrorGroup(list[i].id).isNull())
            list.removeAt(i--);
        // Remove results in a build_output folder
        else if (list[i].filename.contains("build_output"))
            list.removeAt(i--);
    }

    list = ResultsData::sort(list);
    removeDuplicates();

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

    for (int row = 0; row < list.size(); ++row) {
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

QString ResultsData::getErrorGroup(QString id)
{
    if (id.contains("clang-analyzer-alpha.Conversion") ||
        id.contains("clang-analyzer-core.Conversion") ||
        id.contains("Wundefined-fixed-cast") ||
        id.contains("Wfixed-literal-promotion") ||
        id.contains("Wbitfield-constant-conversion") ||
        id.contains("Wshift-overflow") ||
        id.contains("Wshift-count-overflow") ||
        id.contains("Winteger-overflow") ||
        id.contains("Wconstant-conversion") ||
        id == "542" || id == "569" || id == "570" || id == "572" || id == "573" || id == "574" || id == "648") {
        return "Conversion";
    }

    if (id.contains("Wimplicit-function-declaration") ||
        id.contains("Wmissing-declarations") ||
        id == "746") {
        return "Declaration Not Found";
    }

    if (id.contains("Wshift-op-parentheses") ||
        id.contains("Wparentheses") ||
        id == "504") {
        return "Parentheses";
    }

    if (id.contains("Wmacro-redefined") || id == "760") {
        return "Redefined Macro";
    }

    if (id.contains("readability-redundant-declaration") || id == "762" || id == "770") {
        return "Redundant Declaration";
    }

    if (id.contains("Wshadow") || id == "578") {
        return "Shadow";
    }

    if (id.contains("Wuninitialized") ||
        id.contains("clang-analyzer-core.uninitialized.Assign") ||
        id.contains("clang-analyzer-core.CallAndMessage") ||
        id.contains("clang-analyzer-core.UndefinedBinaryOperatorResult") ||
        id == "530" ||
        id == "603" ||
        id == "771" ||
        id == "772") {
        return "Uninitialized";
    }

    if (id.contains("clang-analyzer-alpha.deadcode.UnreachableCode") || id == "527") {
        return "Unreachable Code";
    }

    if (id.contains("Wunused-macro") || id == "750") {
        return "Unused Macro";
    }

    if (id.contains("Wunused-variable") ||
        id == "551" ||
        id == "752") {
        return "Unused Symbol";
    }

    return QString();
}

QStringList ResultsData::getErrorGroupList()
{
    QStringList ret;
    ret.append("Conversion");
    ret.append("Declaration Not Found");
    ret.append("Parentheses");
    ret.append("Redefined Macro");
    ret.append("Redundant Declaration");
    ret.append("Shadow");
    ret.append("Uninitialized");
    ret.append("Unreachable Code");
    ret.append("Unused Macro");
    ret.append("Unused Symbol");
    return ret;
}

bool ResultsData::includeLineInExport(const ResultsData::Line &ln, const QString errorGroup)
{
    return ((errorGroup == "All") || (errorGroup == ResultsData::getErrorGroup(ln.id)));
}

QList<ResultsData::Line> ResultsData::getResultsToExport(const QString errorGroup) const
{
    QList<Line> ret;
    foreach (const Line &line, list) {
        if (!line.filename.isEmpty() && ResultsData::includeLineInExport(line, errorGroup))
            ret.append(line);
    }
    return ret;
}

static bool lessThan(const ResultsData::Line &line1, const ResultsData::Line &line2)
{
    if (line1.filename != line2.filename)
        return line1.filename < line2.filename;
    if (line1.id != line2.id)
        return line1.id < line2.id;
    if (line1.text != line2.text)
        return line1.text < line2.text;
    if (line1.column != line2.column) {
        bool ok1(false);
        bool ok2(false);
        int c1 = line1.column.toInt(&ok1);
        int c2 = line2.column.toInt(&ok2);
        if (ok1 && ok2)
            return c1 < c2;
        return line1.column < line2.column;
    }
    if (line1.line != line2.line)
        return line1.line.toInt() < line2.line.toInt();

    return false;
}

QList<ResultsData::Line> ResultsData::sort(QList<ResultsData::Line> results)
{
    std::sort(results.begin(), results.end(), lessThan);
    return results;
}

void ResultsData::removeDuplicates()
{
    for (int i = 0; i < list.size() - 1; ++i) {
        if (list[i].triage.startsWith("DUP"))
            list.removeAt(i--);
    }
    for (int i = 0; i < list.size() - 1; ++i) {
        if (list[i].filename != list[i+1].filename)
            continue;
        if (list[i].column != list[i+1].column)
            continue;
        if (list[i].text != list[i+1].text)
            continue;
        if (list[i].line != list[i+1].line)
            continue;
        if (list[i].sha.isEmpty() && !list[i+1].sha.isEmpty())
            list[i].sha = list[i+1].sha;
        else if (!list[i].sha.isEmpty() && list[i+1].sha.isEmpty())
            list[i+1].sha = list[i].sha;
        if (list[i].triage.isEmpty())
            list.removeAt(i--);
        else if (list[i+1].triage.isEmpty()) {
            list.removeAt(i + 1);
            i--;
        } else if (list[i].triage == list[i+1].triage)
            list.removeAt(i--);
    }
}

QString ResultsData::report() const
{
    QMap<QString, int> truePositives;
    QMap<QString, int> falsePositives;
    foreach (const QString &errorGroup, ResultsData::getErrorGroupList()) {
        truePositives[errorGroup] = 0;
        falsePositives[errorGroup] = 0;
    }
    foreach (const ResultsData::Line &line, list) {
        if (line.filename.isEmpty())
            continue;
        const QString errorGroup = ResultsData::getErrorGroup(line.id);
        if (line.triage.startsWith("TP"))
            truePositives[errorGroup] += 1;
        else if (line.triage.startsWith("FP"))
            falsePositives[errorGroup] += 1;
    }
    QString ret = "";
    QTextStream ostr(&ret);
    foreach (const QString &errorGroup, truePositives.keys()) {
        ostr << errorGroup << '\t' << truePositives[errorGroup] << '\t' << falsePositives[errorGroup] << '\n';
    }
    return ret;
}

