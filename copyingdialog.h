#ifndef COPYINGDIALOG_H
#define COPYINGDIALOG_H

#include <QDialog>
#include <QProgressDialog>
#include "copythread.h"

namespace Ui {
class CopyingDialog;
}

class CopyingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CopyingDialog(QWidget *parent = 0);
    ~CopyingDialog();

    QStringList outputDirectories() const;
    void setOutputDirectories(const QStringList &outputDirectories);

    QString inputDirectory() const;
    void setInputDirectory(const QString &inputDirectory);

    void beginCopying();

    bool overwritefiles() const;
    void setOverwritefiles(bool overwritefiles);

    bool validateData() const;
    void setValidateData(bool validateData);

    QStringList failedFilesSummary() const;
    void appendFailedFilesSummary(QString summary);
    void setFailedFilesSummary(const QStringList &failedFilesSummary);

    bool formatDrives() const;
    void setFormatDrives(bool formatDrives);

    QString formatDiskLabel() const;
    void setFormatDiskLabel(const QString &formatDiskLabel);

private slots:
    void onFileCopied(bool,QString);
    void onFileValidated(bool, QString);
    void copyingCompleted();
    void on_buttonBox_rejected();

private:
    Ui::CopyingDialog *ui;
    bool formatDrive(QString driveLetter,QString newLabel = "DISK",bool autoYes = false);

    void copyFolder2(QString sourceFolder, QString destFolder);
    int countEntries(QString srcFolder);
    void copyFolder(QString srcFolder, QString destFolder);

    QStringList _outputDirectories;
    QString _inputDirectory;
    bool _overwritefiles;
    bool _validateData;
    bool _formatDrives;
    QString _formatDiskLabel;
    QStringList _failedFilesSummary;

    QList<CopyThread*> CopyThreads;



};

#endif // COPYINGDIALOG_H
