#include "diffdialog.h"
#include "ui_diffdialog.h"
#include "resultsdata.h"
#include "exportdialog.h"
#include <QDebug>


diffDialog::diffDialog(QWidget *parent, QString filename1, QString filename2) :
    QDialog(parent),
    ui(new Ui::diffDialog)
{
    ui->setupUi(this);

    // Tool tips
    ui->closeButton->setToolTip(tr("Close the comparison window."));
    ui->connectButton->setToolTip(tr("Connects two selected lines and adds it in the rule page"));
    ui->deleteButton->setToolTip(tr("Removes the selected rule."));

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_3->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableView->setShowGrid(false);
    ui->tableView_2->setShowGrid(false);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView_2->verticalHeader()->setVisible(false);

    resultsData1 = new ResultsData(filename1);
    resultsData2 = new ResultsData(filename2);
    ResultsData::syncFileNames(resultsData1, resultsData2);

    //createConfigureAndSetRuleView(ui->tableView_3);

    // Handle the automapping suggestions
    //autoMapSuggestions();

    // Set and config input files to views
    setAndConfigureView(ui->tableView, resultsData1->model);
    setAndConfigureView(ui->tableView_2, resultsData2->model);

    // Connect scroll bars !WORKS BUT NOT SO GOOD!
    QObject::connect(ui->tableView_2->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->tableView->verticalScrollBar(), SLOT(setValue(int)));
    QObject::connect(ui->tableView->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->tableView_2->verticalScrollBar(), SLOT(setValue(int)));
    highlighter = new Highlighter(ui->textEdit->document());
}



diffDialog::~diffDialog()
{
    delete ui;
    delete resultsData1;
    delete resultsData2;
}

// --------------------------------------------------------------------------------------
// Privat slot
// --------------------------------------------------------------------------------------

void diffDialog::on_connectButton_clicked()
{/*
    QModelIndexList indexListOne = ui->tableView->selectionModel()->selectedIndexes();
    QModelIndexList indexListTwo = ui->tableView_2->selectionModel()->selectedIndexes();
    if (( indexListOne.isEmpty() && indexListTwo.isEmpty() ) ||( indexListOne.size() > 1 || indexListTwo.size() > 1 )) {
        QMessageBox::information(this, tr("Warning!"), tr("Please select ONE index to connect."));
        return;
    }

    QList<QString> row1, row2;

    // Multiple rows can be selected
    foreach (QModelIndex index, indexListOne) {
        row1.append(QString::number(index.row()));
    }

    // Multiple rows can be selected
    foreach (QModelIndex index, indexListTwo) {
        row2.append(QString::number(index.row()));
    }


    // Appending another row to rules list
    QList<QStandardItem*> newRow;
    QStandardItem *itm = new QStandardItem(1);
    modelListRules->appendRow(itm);

    // Populating the new row with information
    int insertRow = modelListRules->rowCount();
    QModelIndex index;

    if (row1.isEmpty()){
        // Filename
        index = modelListRules->index(insertRow-1, 0, QModelIndex());
        modelListRules->setData(index, model2->data( model2->index(row2.first().toInt(), fileNameColumn, QModelIndex()) ) );

        //SHA
        index = modelListRules->index(insertRow-1, 1, QModelIndex());
        modelListRules->setData(index, "" );

        // First file error msg
        index = modelListRules->index(insertRow-1, 2, QModelIndex());
        modelListRules->setData(index, "" );

        // Row
        index = modelListRules->index(insertRow-1, 3, QModelIndex());
        modelListRules->setData(index, "" );
    } else {
        // Filename
        index = modelListRules->index(insertRow-1, 0, QModelIndex());
        modelListRules->setData(index, model1->data( model1->index(row1.first().toInt(), fileNameColumn, QModelIndex()) ) );

        //SHA
        index = modelListRules->index(insertRow-1, 1, QModelIndex());
        modelListRules->setData(index, model1->data( model1->index(row1.first().toInt(), shaColumn, QModelIndex()) ) );

        // First file error msg
        index = modelListRules->index(insertRow-1, 2, QModelIndex());
        modelListRules->setData(index, model1->data( model1->index(row1.first().toInt(), 3, QModelIndex()) ) );

        // Row
        index = modelListRules->index(insertRow-1, 3, QModelIndex());
        modelListRules->setData(index, model1->data( model1->index(row1.first().toInt(), lineColumn, QModelIndex()) ) );
    }

    if (row2.isEmpty()){
        // SHA
        index = modelListRules->index(insertRow-1, 4, QModelIndex());
        modelListRules->setData(index, "" );

        // Second file error msg
        index = modelListRules->index(insertRow-1, 5, QModelIndex());
        modelListRules->setData(index, "" );

        // Row
        index = modelListRules->index(insertRow-1, 6, QModelIndex());
        modelListRules->setData(index, "" );
    } else {
        // SHA
        index = modelListRules->index(insertRow-1, 4, QModelIndex());
        modelListRules->setData(index, model2->data( model2->index(row2.first().toInt(), shaColumn, QModelIndex()) ) );

        // Second file error msg
        index = modelListRules->index(insertRow-1, 5, QModelIndex());
        modelListRules->setData(index, model2->data( model2->index(row2.first().toInt(), 3, QModelIndex()) ) );

        // Row
        index = modelListRules->index(insertRow-1, 6, QModelIndex());
        modelListRules->setData(index, model2->data( model2->index(row2.first().toInt(), lineColumn, QModelIndex()) ) );
    }

    // Comment
    index = modelListRules->index(insertRow-1, 7, QModelIndex());
    modelListRules->setData(index, ui->lineEdit->text());

    // TP/FP
    index = modelListRules->index(insertRow-1, 8, QModelIndex());
    modelListRules->setData(index, ui->radioTPButton->isChecked() ? "TP" : "FP");

    // Clear input
    ui->tableView->selectionModel()->clear();
    ui->tableView_2->selectionModel()->clear();
    ui->lineEdit->clear();
*/

    // Debug
//    qDebug() << "Row1:" << row1 << "Row2:" << row2;

}


void diffDialog::on_deleteButton_clicked()
{/*
    QModelIndex currentIndex = ui->tableView_3->selectionModel()->currentIndex();
    ui->tableView_3->selectionModel()->clear();
    modelListRules->removeRow(currentIndex.row());
*/}

void diffDialog::on_exportButton_clicked()
{
    ExportDialog exportToFile(this, *resultsData1, *resultsData2);
    exportToFile.exec();
}

static int linePosition(const QString &str, int lineToOpen) {
    int linenr = 1;
    int pos;
    for (pos = 0; linenr < lineToOpen && pos < str.size(); ++pos) {
        if (str[pos] == '\n')
            ++linenr;
    }
    return linenr == lineToOpen ? pos : 0;
}

void diffDialog::openResult(const QModelIndex &index, const ResultsData &resultsData) {
    const QString &fileToOpen = resultsData.list[index.row()].filename;
    int lineToOpen = resultsData.list[index.row()].line.toInt();

    if ( fileToOpen.isEmpty() )
    {
        QMessageBox::information(this, tr("Warning"), tr("No filename selected!"));
        return;
    }

    QFile f(fileToOpen);
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        const QString fileData(f.readAll());
        ui->textEdit->setText(fileData);
        QTextCursor tc = ui->textEdit->textCursor();
        tc.setPosition(linePosition(fileData,lineToOpen));
        ui->textEdit->setTextCursor(tc);
        ui->textEdit->setFocus();
    }
}

void diffDialog::on_tableView_doubleClicked(const QModelIndex &index)
{
    openResult(index, *resultsData1);
}

void diffDialog::on_tableView_2_doubleClicked(const QModelIndex &index)
{
    openResult(index, *resultsData2);
}

void diffDialog::findPrevious() {
    const QString seltext = ui->textEdit->textCursor().selectedText();
    ui->textEdit->find(seltext, QTextDocument::FindBackward | QTextDocument::FindWholeWords);
}

void diffDialog::findNext() {
    const QString seltext = ui->textEdit->textCursor().selectedText();
    ui->textEdit->find(seltext, QTextDocument::FindWholeWords);
}

// --------------------------------------------------------------------------------------
// Privat
// --------------------------------------------------------------------------------------


void diffDialog::setAndConfigureView(QTableView *tabView, QStandardItemModel *model)
{
    tabView->setModel(model);

    for ( int column = 0; column < model->columnCount(); ++column )
        tabView->setColumnWidth(column, 80);

    if (ResultsData::getFilenameColumn() >= 0)
        tabView->setColumnWidth(ResultsData::getFilenameColumn(), 200);
    if (ResultsData::getLineColumn() >= 0)
        tabView->setColumnWidth(ResultsData::getLineColumn(), 60);
    if (ResultsData::getTextColumn() >= 0)
        tabView->setColumnWidth(ResultsData::getTextColumn(), 200);
    if (ResultsData::getTriageColumn() >= 0)
        tabView->setColumnWidth(ResultsData::getTriageColumn(), 60);
}


void diffDialog::createConfigureAndSetRuleView(QTableView *tabView)
{
//    ui->lineEdit->setPlaceholderText("Add comments...");

    modelListRules = new QStandardItemModel(0, 9, this);
    tabView->setModel(modelListRules);
    tabView->resizeColumnsToContents();
    tabView->setColumnWidth(0, 3*(this->width()/9));
    tabView->setColumnWidth(1, 1*(this->width()/9));
    tabView->setColumnWidth(2, 4*(this->width()/9));
    tabView->setColumnWidth(3, 0.5*(this->width()/9));
    tabView->setColumnWidth(4, 1*(this->width()/9));
    tabView->setColumnWidth(5, 4*(this->width()/9));
    tabView->setColumnWidth(6, 0.5*(this->width()/9));
    tabView->setColumnWidth(7, 3*(this->width()/9));
    tabView->setColumnWidth(8, 0.5*(this->width()/9));

//    tabView->horizontalHeader()->setStretchLastSection(true);

    modelListRules->setHeaderData(0, Qt::Horizontal, "File name");
    modelListRules->setHeaderData(1, Qt::Horizontal, "SHA 1");
    modelListRules->setHeaderData(2, Qt::Horizontal, resultsData1->fileName);
    modelListRules->setHeaderData(3, Qt::Horizontal, "Row");

    modelListRules->setHeaderData(4, Qt::Horizontal, "SHA 2");
    modelListRules->setHeaderData(5, Qt::Horizontal, resultsData2->fileName);
    modelListRules->setHeaderData(6, Qt::Horizontal, "Row");
    modelListRules->setHeaderData(7, Qt::Horizontal, "Comments");
    modelListRules->setHeaderData(8, Qt::Horizontal, "TP/FP");

}


void diffDialog::autoMapSuggestions()
{
/*
    for (int rowmodel1 = 0; rowmodel1 < model1->rowCount(); rowmodel1++)
    {
        for (int rowmodel2 = 0; rowmodel2 < model2->rowCount(); rowmodel2++)
        {
            if (model1->data( model1->index(rowmodel1, fileNameColumn, QModelIndex()) ).toString().isEmpty() ||
                model2->data( model2->index(rowmodel2, fileNameColumn, QModelIndex()) ).toString().isEmpty()){
                // Do nothing the row is empty
            } else {
                if ( model1->data( model1->index(rowmodel1, shaColumn, QModelIndex()) ).toString() ==
                     model2->data( model2->index(rowmodel2, shaColumn, QModelIndex()) ).toString() &&
                     model1->data( model1->index(rowmodel1, fileNameColumn, QModelIndex()) ).toString() ==
                     model2->data( model2->index(rowmodel2, fileNameColumn, QModelIndex()) ).toString() &&
                    ( model1->data( model1->index(rowmodel1, lineColumn, QModelIndex()) ).toInt() >
                      model2->data( model2->index(rowmodel2, lineColumn, QModelIndex()) ).toInt() - 100 &&
                      model1->data( model1->index(rowmodel1, lineColumn, QModelIndex()) ).toInt() <
                      model2->data( model2->index(rowmodel2, lineColumn, QModelIndex()) ).toInt() + 100)
                      )
                {
                    if (autoMap.contains(rowmodel1)){
                        //TODO: Check if we have a closer match
                    } else {
                        autoMap[rowmodel1] = rowmodel2;

                    }
                }
            }
        }
    }
    int i = 0, myRGB = 150;
    foreach (int k, autoMap.keys()) {
//        qDebug() << "Key:" << k << "Value:" << autoMap[k];
        if (i%2 == 0)
            myRGB = 230;
        else
            myRGB = 160;
        model1->setData( model1->index(k, fileNameColumn, QModelIndex()), QVariant(QBrush(qRgb(myRGB,myRGB,myRGB))), Qt::BackgroundRole );
        model2->setData( model2->index(autoMap[k], fileNameColumn, QModelIndex()), QVariant(QBrush(qRgb(myRGB,myRGB,myRGB))), Qt::BackgroundRole );
        i++;
    }
*/
}

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    numberFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("[0-9]x?[.0-9]*");
    rule.format = numberFormat;
    highlightingRules.append(rule);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);


    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
