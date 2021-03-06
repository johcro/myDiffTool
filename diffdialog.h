#ifndef DIFFDIALOG_H
#define DIFFDIALOG_H

#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QStringList>
#include <QTableWidget>
#include <QRegExp>
#include <QSyntaxHighlighter>
#include <QtCore>
#include <QtGui>
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>

namespace Ui {
    class diffDialog;
}

class ResultsData;

class Highlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) /*Q_DECL_OVERRIDE*/;

private:
    struct HighlightingRule {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat functionFormat;
};

class diffDialog : public QDialog {
    Q_OBJECT

public:
    explicit diffDialog(QWidget *parent = 0,  QString filename1 = 0, QString filename2 = 0);
    ~diffDialog();

private slots:

    void on_connectButton_clicked();

    void on_deleteButton_clicked();

    void on_exportButton_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_tableView_2_doubleClicked(const QModelIndex &index);

    void report();

    void findPrevious();
    void findNext();

private:
    Ui::diffDialog *ui;
    QStandardItemModel *modelListRules;

    void setAndConfigureView(QTableView *tabView, QStandardItemModel *model);
    void createConfigureAndSetRuleView(QTableView *tabView);
    void autoMapSuggestions();
    void openResult(const QModelIndex &index, const ResultsData &resultsData);

    Highlighter *highlighter;

    ResultsData *resultsData1;
    ResultsData *resultsData2;

    QMap<int, int> autoMap;
};

#endif // DIFFDIALOG_H
