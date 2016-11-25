#include "exportdialog.h"
#include "ui_exportdialog.h"
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include "resultsdata.h"

ExportDialog::ExportDialog(QWidget *parent, ResultsData *clangResults, ResultsData *lintResults) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);

    if ((clangResults == NULL) || (lintResults == NULL) )
        return;

    clangData = clangResults->list;
    lintData = lintResults->list;

}

ExportDialog::~ExportDialog()
{
    delete ui;
}

bool ExportDialog::includeLineInExport(ResultsData::Line *ln)
{
    bool returnData(false);

    if (errorGroup == "All")
    {
        /* This line shall be included in export, goto next line. */
        returnData = true;
    }
    else if (errorGroup == "Conversion")
    {
        if (ln->id.contains("clang-analyzer-alpha.Conversion") ||
                ln->id.contains("570") || ln->id.contains("573") ||
                ln->id.contains("574") || ln->id.contains("648"))
        {
            /* This line shall be included in export, goto next line. */
            returnData = true;
        }
    }
    else if (errorGroup == "Declaration Not Found")
    {
        if (ln->id.contains("Wimplicit-function-declaration") ||
                ln->id.contains("Wmissing-declarations") ||
                ln->id.contains("746"))
        {
            /* This line shall be included in export, goto next line. */
            returnData = true;
        }
    }

    else if (errorGroup == "Loss Of Precision")
    {
        if (ln->id.contains("Wundefined-fixed-cast") ||
                ln->id.contains("Wfixed-literal-promotion") ||
                ln->id.contains("Wbitfield-constant-conversion") ||
                ln->id.contains("542"))
        {
            /* This line shall be included in export, goto next line. */
            returnData = true;
        }
    }

    else if (errorGroup == "Redefined Macro")
    {
        if (ln->id.contains("Wunused-macro") ||
                ln->id.contains("760"))
        {
            /* This line shall be included in export, goto next line. */
            returnData = true;
        }
    }

    else if (errorGroup == "Redundant Declaration")
    {
        if (ln->id.contains("readability-redundant-declaration") ||
                ln->id.contains("762"))
        {
            /* This line shall be included in export, goto next line. */
            returnData = true;
        }
    }

    else if (errorGroup == "Shadow")
    {
        if (ln->id.contains("Wshadow") ||
                ln->id.contains("578"))
        {
            /* This line shall be included in export, goto next line. */
            returnData = true;
        }
    }

    else if (errorGroup == "Uninitialized")
    {
        if (ln->id.contains("Wuninitialized") ||
                ln->id.contains("clang-analyzer-core.uninitialized.Assign") ||
                ln->id.contains("clang-analyzer-core.CallAndMessage") ||
                ln->id.contains("clang-analyzer-core.UndefinedBinaryOperatorResult") ||
                ln->id.contains("530")  ||
                ln->id.contains("603")  ||
                ln->id.contains("771"))
        {
            /* This line shall be included in export, goto next line. */
            returnData = true;
        }
    }

    else if (errorGroup == "Unreachable Code")
    {
        if (ln->id.contains("clang-analyzer-alpha.deadcode.UnreachableCode") ||
                ln->id.contains("527"))
        {
            /* This line shall be included in export, goto next line. */
            returnData = true;
        }
    }

    else if (errorGroup == "Unused Macro")
    {
        if (ln->id.contains("Wunused-macro") ||
                ln->id.contains("750"))
        {
            /* This line shall be included in export, goto next line. */
            returnData = true;
        }
    }

    else if (errorGroup == "Unused Value")
    {
        if (ln->id.contains("Wunused-variable") ||
                ln->id.contains("Wunused-value") ||
                ln->id.contains("551"))
        {
            /* This line shall be included in export, goto next line. */
            returnData = true;
        }
    }
    else
    {
        QMessageBox::information(this, tr("Error"), tr("Unknown error group selected!"));
    }

    return returnData;
}

void ExportDialog::on_pushButton_clicked()
{
    //Export

    QString whichTool = ui->comboBox_2->currentText();
    QList<ResultsData::Line> resultsToExport;
    errorGroup = ui->comboBox->currentText();

    /* Setup which tool output to export */
    if (whichTool == "Clang")
    {
        resultsToExport = clangData;
    }
    else if (whichTool == "Lint")
    {
        resultsToExport = lintData;
    }
    else
    {
        /* No tool selected */
        QMessageBox::information(this, tr("Warning"), tr("No tool was selected!"));
        return;
    }

    /* If no error group is selected, end */
    if (errorGroup == "None")
    {
        QMessageBox::information(this, tr("Warning"), tr("No error/warning to export selected!"));
        return;
    }

    /*******************************
     * Start the sorting and export
     *******************************/
    QString defaultResultFile = QDir::homePath() + "/" + whichTool + "-" + errorGroup + " Warnings" ;
    QString resFileName = QFileDialog::getSaveFileName(this, tr("Save export as"), defaultResultFile, tr("Text Files (*.txt)"));

    /* The list containing the sorted results */
    QList<UnsortedResults> res;

    if ( resFileName.isEmpty() )
    {
        QMessageBox::information(this, tr("Warning"), tr("No export filename selected!"));
        return;
    }

    QFile f(resFileName);

    if (f.open(QFile::WriteOnly | QFile::Text))
    {

        foreach (ResultsData::Line ln, resultsToExport)
        {
            if (ln.filename.isEmpty())
            {
                /* Skip empty lines caused by the diff matching algorithm */
                continue;
            }
            else if (includeLineInExport(&ln) == true)
            {
                /* Make sure that empty triage is tagged UNKNOWN */
                if (ln.triage.isEmpty())
                {
                    ln.triage = "UNKNOWN:(New)";
                }
                else if (ln.triage.contains("UNKNOWN:(New)", Qt::CaseInsensitive))
                {
                    /* Not a new unknown any longer, change to only unknown */
                    ln.triage = "UNKNOWN:";
                }

                QString line;

                if (whichTool == "Lint")
                {
                    /* Lint warnings contains no column or severity */
                    if (ln.sha.isEmpty())
                    {
                        line = ln.triage + ";" + ln.filename + ";" +
                                ln.line + ";" + ln.text + ";[" + ln.id + "]\n";
                    }
                    else
                    {
                        line = ln.triage + ";" + ln.sha + "/" + ln.filename + ";" +
                                ln.line + ";" + ln.text + ";[" + ln.id + "]\n";
                    }
                }
                else if (whichTool == "Clang")
                {
                    if (ln.sha.isEmpty())
                    {
                        line = ln.triage + ";" + ln.filename + ";" +
                                ln.line + ";" + ln.column + ";" +
                                ln.severity + ";" + ln.text + ";[" +
                                ln.id + "]\n";
                    }
                    else
                    {
                        line = ln.triage + ";" + ln.sha + "/" + ln.filename + ";" +
                                ln.line + ";" + ln.column + ";" +
                                ln.severity + ";" + ln.text + ";[" +
                                ln.id + "]\n";
                    }
                }

                /* Sort according to following prio
                 * 1: Triage (TP>FP>Unknown)
                 * 2: Filename
                 * 3: Id
                 * 4: Line
                 * 5: Column
                 */

                UnsortedResults tmpItem;
                tmpItem.info = ln;
                tmpItem.ln = line;

                if (res.isEmpty())
                {
                    /* First item do not need to be sorted. */
                    res.append(tmpItem);
                }
                else
                {
                    bool sorted = false;
                    int j = 0;
                    for (int i = 0; i < res.count(); i++)
                    {
                        /* Loop up to first item with matching Triage. */

                        if ((res[i].info.triage.startsWith("TP", Qt::CaseInsensitive) && tmpItem.info.triage.startsWith("TP", Qt::CaseInsensitive)) ||
                                (res[i].info.triage.startsWith("FP", Qt::CaseInsensitive) && tmpItem.info.triage.startsWith("FP", Qt::CaseInsensitive)) ||
                                (res[i].info.triage.startsWith("UNKNOWN", Qt::CaseInsensitive) && tmpItem.info.triage.startsWith("UNKNOWN", Qt::CaseInsensitive)))
                        {
                            /* Found first item in list with matching triage */
                            j = i;
                            break;
                        }
                        else if ((tmpItem.info.triage.startsWith("TP", Qt::CaseInsensitive) &&
                                  (res[i].info.triage.startsWith("FP", Qt::CaseInsensitive) ||
                                   res[i].info.triage.startsWith("UNKNOWN", Qt::CaseInsensitive)))
                                 ||
                                 (tmpItem.info.triage.startsWith("FP", Qt::CaseInsensitive) &&
                                  res[i].info.triage.startsWith("UNKNOWN", Qt::CaseInsensitive)))
                        {
                            /* Triages of lower prio found, insert before */
                            res.insert(i, tmpItem);
                            sorted = true;
                            break;
                        }
                        else if (i == (res.count() - 1))
                        {
                            /* Ended up last in list */
                            res.append(tmpItem);
                            sorted = true;
                            break;
                        }
                    }

                    if (!sorted)
                    {
                        QString triageTag = tmpItem.info.triage.startsWith("TP", Qt::CaseInsensitive) ? "TP" : tmpItem.info.triage.startsWith("FP", Qt::CaseInsensitive) ? "FP" : "UNKNOWN";

                        while (j < res.count())
                        {
                            if (!res[j].info.triage.startsWith(triageTag, Qt::CaseInsensitive))
                            {
                                /* Triage do not match any longer, add before as last in matching triage. */
                                res.insert(j, tmpItem);
                                sorted = true;
                                break;
                            }
                            else if (res[j].info.filename < tmpItem.info.filename)
                            {
                                /* Matching filename not found, continue */
                                j++;
                            }
                            else if (res[j].info.filename > tmpItem.info.filename)
                            {
                                /* Passed matching filename, insert before */
                                res.insert(j, tmpItem);
                                sorted = true;
                                break;
                            }
                            else if ((whichTool == "Clang") && (res[j].info.id < tmpItem.info.id))
                            {
                                /* Matching id not found, continue */
                                j++;
                            }
                            else if ((whichTool == "Clang") && (res[j].info.id > tmpItem.info.id))
                            {
                                /* Passed matching id, insert before */
                                res.insert(j, tmpItem);
                                sorted = true;
                                break;
                            }
                            else if ((whichTool == "Lint") && (res[j].info.id.toInt() < tmpItem.info.id.toInt()))
                            {
                                /* Matching id not found, continue */
                                j++;
                            }
                            else if ((whichTool == "Lint") && (res[j].info.id.toInt() > tmpItem.info.id.toInt()))
                            {
                                /* Passed matching id, insert before */
                                res.insert(j, tmpItem);
                                sorted = true;
                                break;
                            }
                            else if (res[j].info.line.toInt() < tmpItem.info.line.toInt())
                            {
                                /* Matching filename not found, continue */
                                j++;
                            }
                            else if (res[j].info.line.toInt() > tmpItem.info.line.toInt())
                            {
                                /* Passed matching filename, insert before */
                                res.insert(j, tmpItem);
                                sorted = true;
                                break;
                            }
                            else if ((whichTool == "Clang") && (res[j].info.column.toInt() < tmpItem.info.column.toInt()))
                            {
                                /* Matching column not found, continue */
                                j++;
                            }
                            else if ((whichTool == "Clang") && (res[j].info.column.toInt() > tmpItem.info.column.toInt()))
                            {
                                /* Passed matching column, insert before */
                                res.insert(j, tmpItem);
                                sorted = true;
                                break;
                            }
                            else
                            {
                                /* Duplicate found, ignore new item */
                                break;
                            }

                            if (j == res.count())
                            {
                                /* Add item last in list if j since reached end of list */
                                res.append(tmpItem);
                                sorted = true;
                                break;
                            }

                        } //while
                    } // sorted

                } //not empty list to add item to.

            } //include this line
        } //foreach line

        /***********************************************
         *  Write entire sorted list to the export file
         ***********************************************/
//        f.write("Triage;Filename;Line;Column;Severity;Text;ID\n");
        foreach (ExportDialog::UnsortedResults Itm, res)
        {
            f.write(Itm.ln.toLatin1());
        }
        f.flush();
        f.close();

    } // open file
    else
    {
        QMessageBox::information(this, tr("Error"), tr("Failed to open export file for writing!"));
        return;
    }

    /***********************************************************
     *  Create a new input file.
     ***************************/
    QString defaultSrcFile = QDir::homePath() + "/" + whichTool + "-new_input_file-" + errorGroup + ".txt";
    QString srcFileName = QFileDialog::getSaveFileName(this, tr("Save the new inputfile as"), defaultSrcFile, tr("Text Files (*.txt)"));

    if ( srcFileName.isEmpty() )
    {
        QMessageBox::information(this, tr("Warning"), tr("No filename selected!"));
        return;
    }

    QFile src_f(srcFileName);

    if (src_f.open(QFile::WriteOnly | QFile::Text))
    {
        QString ln1("");

        foreach (UnsortedResults r, res)
        {
            ln1.clear();

            if (!r.info.sha.isEmpty())
            {
                ln1.append(r.info.sha);
            }
            ln1.append(r.info.filename + ":");
            ln1.append(r.info.line + ":");
            if (whichTool == "Clang")
            {
                ln1.append(r.info.column + ":" + r.info.severity + ":");
            }
            else if (whichTool == "Lint")
            {
                /* Lint lacks column and severity */
                ln1.append("");
            }
            ln1.append(r.info.text + "[" + r.info.id + "]\n" + r.info.triage + "\n");
            src_f.write(ln1.toLatin1());
        }
        src_f.flush();
        src_f.close();
    } // open file
    else
    {
        QMessageBox::information(this, tr("Error"), tr("Failed to open the new input file for writing!"));
        return;
    }

}

