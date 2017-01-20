#include "copyingdialog.h"
#include "ui_copyingdialog.h"
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFutureWatcher>
#include <QtConcurrent>
#include "copythread.h"
#include <QMessageBox>
#include "copyprogress.h"
#include <QTimer>
#include <QColor>
#include <QProcess>

CopyingDialog::CopyingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CopyingDialog)
{
    ui->setupUi(this);
    QCoreApplication::processEvents();
    setOverwritefiles(false); //Should files be overwritten?
    setValidateData(true); //Validate data
    QCoreApplication::processEvents();
}

CopyingDialog::~CopyingDialog()
{
    delete ui;
}

QStringList CopyingDialog::outputDirectories() const
{
    return _outputDirectories;
}

void CopyingDialog::setOutputDirectories(const QStringList &outputDirectories)
{
    _outputDirectories = outputDirectories;
}

QString CopyingDialog::inputDirectory() const
{
    return _inputDirectory;
}

void CopyingDialog::setInputDirectory(const QString &inputDirectory)
{
    _inputDirectory = inputDirectory;
}


void copyFolder2(QString sourceFolder, QString destFolder)
{
    QDir sourceDir(sourceFolder);
    if(!sourceDir.exists())
        return;
    QDir destDir(destFolder);
    if(!destDir.exists())
    {
        destDir.mkdir(destFolder);
    }
    QStringList files = sourceDir.entryList(QDir::Files);
    for(int i = 0; i< files.count(); i++)
    {
        QString srcName = sourceFolder + "/" + files[i];
        QString destName = destFolder + "/" + files[i];
        QFile::copy(srcName, destName);
    }
    files.clear();
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for(int i = 0; i< files.count(); i++)
    {
        QString srcName = sourceFolder + "/" + files[i];
        QString destName = destFolder + "/" + files[i];
        //copyFolder(srcName, destName);
    }
}

bool CopyingDialog::formatDrive(QString driveLetter, QString newLabel,bool autoYes)
{
    if(newLabel.size()>10)
    {
        qDebug() << "New label is too long, must only be 11 characters";
    }

    //Checks if drive Exists
    QDir drive(driveLetter);
    if(!drive.exists())
    {
        qDebug() << "Drive" << driveLetter << "does not exists!";
        return false;
    }

    //Removes slashes from drive letter, if any so D:/ => D:
    driveLetter = driveLetter.remove("\\").remove("/");

    QProcess shell;
    QStringList arguments;
    arguments << "/K" << "FORMAT" << driveLetter << "/FS:FAT32" << "/Q" << "/X" << "/V:nFDisk";
    if(autoYes) arguments.append("/y");
    arguments << "&&" << "LABEL" << driveLetter << newLabel;
    arguments << "&&" << "exit";
    if(shell.execute("cmd.exe",arguments) == 0)
    {
        qDebug() << "is Completed";
        return true;
    }
    return false;
}


int CopyingDialog::countEntries(QString srcFolder)
{
    //Checks if source exists
    QDir sourceDir(srcFolder);
    if(!sourceDir.exists())
    {
        qDebug() << "Source directory does not exists!";
        return -1;
    }

    //Resets the fileCount
    int filesCount = 0;

    //Finds alle subDirs
    QStringList subDirs = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    foreach(QString subDir, subDirs)
    {
        QString src = QDir::toNativeSeparators(srcFolder + "/" + subDir);
        qDebug() << "Entering folder" << subDir;
        filesCount = filesCount + countEntries(src);
    }

    //Finds all files in sourcedir
    sourceDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    filesCount = filesCount + sourceDir.count();

    return filesCount;
}

QString CopyingDialog::formatDiskLabel() const
{
    return _formatDiskLabel;
}

void CopyingDialog::setFormatDiskLabel(const QString &formatDiskLabel)
{
    _formatDiskLabel = formatDiskLabel;
}

bool CopyingDialog::formatDrives() const
{
    return _formatDrives;
}

void CopyingDialog::setFormatDrives(bool formatDrives)
{
    _formatDrives = formatDrives;
}

QStringList CopyingDialog::failedFilesSummary() const
{
    return _failedFilesSummary;
}

void CopyingDialog::appendFailedFilesSummary(QString summary)
{
    _failedFilesSummary.append(summary);
}

void CopyingDialog::setFailedFilesSummary(const QStringList &failedFilesSummary)
{
    _failedFilesSummary = failedFilesSummary;
}

bool CopyingDialog::validateData() const
{
    return _validateData;
}

void CopyingDialog::setValidateData(bool validateData)
{
    _validateData = validateData;
}

bool CopyingDialog::overwritefiles() const
{
    return _overwritefiles;
}

void CopyingDialog::setOverwritefiles(bool overwritefiles)
{
    _overwritefiles = overwritefiles;
}

void CopyingDialog::beginCopying()
{
    qDebug() << "Copy process started";

    //Progressbars are reset
    //ui->subProgressBar->setValue(0);
    ui->totalProgressBar->setValue(0);

    //totalProgressbars max value are set to count of outputdirs.
    ui->totalProgressBar->setMaximum(outputDirectories().count());

    QMessageBox *pEntriesCount = new QMessageBox(this);
    pEntriesCount->setText(tr("Counting files in source directory"));
    pEntriesCount->setStandardButtons(0);
    pEntriesCount->move(this->x(),this->y());
    pEntriesCount->show();
    QCoreApplication::processEvents();
    int entriesCount = countEntries(inputDirectory());
    pEntriesCount->hide();
    QCoreApplication::processEvents();
    //ui->subProgressBar->setMaximum(entriesCount);

    foreach(QString outputDirectory, outputDirectories())
    {
        if(formatDrives())
        {
            QString diskLabel = formatDiskLabel();
            if(diskLabel.isEmpty() || diskLabel == "")
            {
                diskLabel = "DISK";
            }
            if(diskLabel.size() > 10)
            {
                diskLabel = diskLabel.left(10);
            }

            ui->log->append(tr("Formating drive %1. Program might freeze up momentarily.").arg(outputDirectory));
            QCoreApplication::processEvents();
            if(!formatDrive(outputDirectory,diskLabel,true))
            {
                qDebug() << "Failed to format drive" << outputDirectory << ". Aborting entire process!";
                ui->log->append(tr("Failed to format drive %1. Aborting entire process!").arg(outputDirectory));
                ui->totalProgressBar->setValue(ui->totalProgressBar->maximum());
                QCoreApplication::processEvents();
                return;
            }
        }
        QCoreApplication::processEvents();
    }

    foreach(QString outputDirectory, outputDirectories())
    {
        //Creates progressbar and connections
        CopyProgress *mCopyProgress = new CopyProgress(this);
        mCopyProgress->setHeaderLabel(outputDirectory);

        //Creates the thread, and connections
        CopyThread *mCopyThread = new CopyThread();

        //Appends to list, for later use
        CopyThreads.append(mCopyThread);

        //Connections
        connect(mCopyProgress,SIGNAL(cancelProgress()),mCopyThread,SLOT(cancelProgress()));
        connect(mCopyThread,SIGNAL(completed()),mCopyProgress,SLOT(progressCompleted()));
        connect(mCopyThread,SIGNAL(filesCopied(int)),mCopyProgress,SLOT(setProgressValue(int)));
        connect(mCopyThread,SIGNAL(onFileCopied(bool,QString)),this,SLOT(onFileCopied(bool, QString)));
        connect(mCopyThread,SIGNAL(onFileValidated(bool,QString)),this,SLOT(onFileValidated(bool,QString)));
        connect(mCopyThread,SIGNAL(completed()),this,SLOT(copyingCompleted()));
        connect(mCopyThread,SIGNAL(completed()),mCopyThread,SLOT(deleteLater()));

        //Sets the values of the progressbar and adds to widget.
        mCopyProgress->setProgressMaximumValue(entriesCount);
        ui->copyProgressLayout->addWidget(mCopyProgress);

        //Sets values for the thread and starts it.
        qDebug() << "OVERWRITE FILES" << overwritefiles();
        mCopyThread->setValidateData(validateData());
        mCopyThread->setOverwritefiles(overwritefiles());
        mCopyThread->setDestinationFolder(outputDirectory);
        mCopyThread->setSourceFolder(inputDirectory());
        mCopyThread->start();
    }
}

void CopyingDialog::onFileCopied(bool success, QString absoluteFilePath)
{
    if(success)
    {
        ui->log->setTextColor(QColor( "green" ));
      ui->log->append(tr("Copy successful: %1").arg(absoluteFilePath));
    }
    else
    {
        ui->log->setTextColor(QColor( "red" ));
        appendFailedFilesSummary(tr("Copy unsuccessful: %1").arg(absoluteFilePath));
        ui->log->append(tr("Copy unsuccessful: %1").arg(absoluteFilePath));
    }
}

void CopyingDialog::onFileValidated(bool valid, QString filename)
{
    if(valid)
    {
        ui->log->setTextColor(QColor( "blue" ));
        ui->log->append(tr("Validation successful: %1").arg(filename));
    }
    else
    {
        ui->log->setTextColor(QColor( "red" ));
        appendFailedFilesSummary(tr("Validation unsuccessful: %1").arg(filename));
        ui->log->append(tr("Validation unsuccessful: %1").arg(filename));
    }
}

void CopyingDialog::copyingCompleted()
{   
//    bool threadsStillRunning = false;
//    foreach(CopyThread *thread,CopyThreads)
//    {
//        if(thread->isRunning())
//        {
//            threadsStillRunning = true;
//        }
//    }

//    if(!threadsStillRunning)
//    {
//        ui->log->append("");
//        ui->log->append("");
//        ui->log->append("..::"+tr("SUMMARY OF ERRORS")+"::..");
//        foreach(QString failedFile, failedFilesSummary())
//        {
//            ui->log->append(failedFile);
//        }
//    }

    ui->totalProgressBar->setValue(ui->totalProgressBar->value()+1);
}


void CopyingDialog::on_buttonBox_rejected()
{
//    QMessageBox msgbox;
//    msgbox.setText(tr("Waiting for copying processes to stop! Please wait."));
//    msgbox.setStandardButtons(0);
//    msgbox.show();

    foreach(CopyThread *thread, CopyThreads)
    {        
//        if(!thread->isRunning())
//        {
//            break;
//        }

        thread->cancelProgress();

//        QTimer stoppingTimer;
//        stoppingTimer.setSingleShot(true);
//        stoppingTimer.setInterval(10000);
//        stoppingTimer.start();

//        while(thread->isRunning())
//        {
//            if(!stoppingTimer.isActive())
//            {
//                qDebug() << "Stopping process timeout, force stop!";
//                thread->quit();
//                break;
//            }

//            QCoreApplication::processEvents();
//            qDebug() << "Waiting for process " << thread << "to stop";
//        }
//        qDebug() << "Thread" << thread << "stopped!";
    }

    //msgbox.hide();
}
