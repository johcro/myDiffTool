#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QtGui>
#include "resultsdata.h"

namespace Ui {
    class ExportDialog;
}

class ExportDialog : public QDialog {
    Q_OBJECT

public:
    explicit ExportDialog(QWidget *parent, const ResultsData &clangResults, const ResultsData &lintResults);
    ~ExportDialog();

private slots:
    void on_pushButton_clicked();

private:

    struct UnsortedResults {
        ResultsData::Line info;
        QString ln;
    };


    Ui::ExportDialog *ui;
    const ResultsData &clangData;
    const ResultsData &lintData;
    QString errorGroup;

};

#endif // EXPORTDIALOG_H
