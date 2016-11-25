#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QtGui>
#include "resultsdata.h"

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(QWidget *parent = 0, ResultsData *clangResults = NULL, ResultsData *lintResults = NULL);
    ~ExportDialog();

private slots:
    void on_pushButton_clicked();

private:

    struct UnsortedResults {
        ResultsData::Line info;
        QString ln;
    };


    bool includeLineInExport(ResultsData::Line *ln);
    Ui::ExportDialog *ui;
    QList<ResultsData::Line> clangData;
    QList<ResultsData::Line> lintData;
    QString errorGroup;

};

#endif // EXPORTDIALOG_H
