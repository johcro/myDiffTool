#include "exportdialog.h"
#include "ui_exportdialog.h"
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include "resultsdata.h"

static const QString TOOL_CLANG("Clang");
static const QString TOOL_LINT("Lint");

ExportDialog::ExportDialog(QWidget *parent, const ResultsData &clangResults, const ResultsData &lintResults) :
    QDialog(parent),
    ui(new Ui::ExportDialog),
    clangData(clangResults),
    lintData(lintResults)
{
    ui->setupUi(this);

    ui->whichTool->clear();
    ui->whichTool->addItem("None");
    ui->whichTool->addItem(TOOL_CLANG);
    ui->whichTool->addItem(TOOL_LINT);

    ui->errorGroup->clear();
    ui->errorGroup->addItem("None");
    ui->errorGroup->addItem("All");
    ui->errorGroup->addItems(ResultsData::getErrorGroupList());
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::on_pushButton_clicked()
{
    //Export

    QString whichTool = ui->whichTool->currentText();
    const QString errorGroup = ui->errorGroup->currentText();
    QList<ResultsData::Line> resultsToExport;

    /* Setup which tool output to export */
    if (whichTool == TOOL_CLANG)
    {
        resultsToExport = clangData.getResultsToExport(errorGroup);
    }
    else if (whichTool == TOOL_LINT)
    {
        resultsToExport = lintData.getResultsToExport(errorGroup);
    }
    else
    {
        /* No tool selected */
        QMessageBox::information(this, tr("Warning"), tr("No tool was selected!"));
        return;
    }

    resultsToExport = ResultsData::sort(resultsToExport);

    /*******************************
     * Start the sorting and export
     *******************************/
    QString defaultResultFile = QDir::homePath() + "/" + whichTool + "-" + errorGroup + " Warnings" ;
    QString resFileName = QFileDialog::getSaveFileName(this, tr("Save export as"), defaultResultFile, tr("Text Files (*.txt)"));
    if ( resFileName.isEmpty() )
    {
        QMessageBox::information(this, tr("Warning"), tr("No export filename selected!"));
        return;
    }

    QFile f(resFileName);
    if (!f.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::information(this, tr("Error"), tr("Failed to open export file for writing!"));
        return;
    }

    QTextStream outStream(&f);

    foreach (const ResultsData::Line &line, resultsToExport)
    {
        if (!line.sha.isEmpty())
            outStream << line.sha << ':';
        outStream << line.filename << ':';
        outStream << line.line << ':';
        if (!line.column.isEmpty())
            outStream << line.column << ':';
        outStream << line.severity << ':';
        outStream << line.text;
        outStream << " [" << line.id << ']';
        if (!line.triage.isEmpty())
            outStream << '\n' << line.triage;
        outStream << '\n';
    }
    f.flush();
    f.close();
}
