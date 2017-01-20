#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStorageInfo>
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QStorageInfo>
#include <QFileDialog>
#include <copyingdialog.h>
#include <QProcess>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Finds the drives, and updates GUI-List
    updateDrivesList();

    ui->formatedDiskLabel->setVisible(false);
    upstartWarning();

    ui->beginCopyingButton->setEnabled(false); //Begin copying button should be disabled at startup.
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateDrivesList()
{
    //Clears the list
    ui->drivesList->clear();

    //Appends to list
    if(ui->viewOnlyMountedVolumes->isChecked())
    {
        if(QStorageInfo::mountedVolumes().count() <= 0)
        {
            ui->drivesList->setEnabled(false);
            ui->drivesList->insertItem(0,tr("No mounted drives found!"));
        }

        foreach(QStorageInfo storage, QStorageInfo::mountedVolumes())
        {
            if (storage.isValid() && storage.isReady()) {
                if (!storage.isReadOnly()) {

                    //If path not containg WINDOWS directory
                    QString windowsPath = storage.rootPath()+QString("/Windows");
                    QDir windowsDir(windowsPath);
                    if(!windowsDir.exists())
                    {
                        int row = ui->drivesList->count();
                        ui->drivesList->insertItem(row,storage.rootPath());
                    }

                }
            }
        }
    }
    else
    {
        foreach(QFileInfo drive,QDir::drives())
        {
            int row = ui->drivesList->count();
            ui->drivesList->insertItem(row,drive.absolutePath());
        }
    }

    //If not drives found
    ui->drivesList->setEnabled(true);
    if(ui->drivesList->count() <= 0)
    {
        ui->drivesList->insertItem(0,tr("No drives found. Insert some and press '%1'").arg(ui->refreshListButton->text()));
        ui->drivesList->setEnabled(false);
    }

}

void MainWindow::upstartWarning()
{    QMessageBox msgbox;
     msgbox.setWindowTitle(tr("CopyToDisk - Upstart warning"));
     msgbox.setIcon(QMessageBox::Warning);
     msgbox.setText(tr("Proceed with caution. All use at own risk!"));
     msgbox.exec();

}

void MainWindow::on_inputBrowseButton_clicked()
{
    //Gets the current path
    QString currentPath = ui->inputBrowsePath->text();

    //Find not set, then set to users homepath like c:\users\<username>
    if(currentPath.isEmpty())
    {
        currentPath = QDir::homePath();
    }

    //Gets the new selection
    QString newPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                currentPath,
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);

    //Sets the new/updated value if newPath not empty
    if(!newPath.isEmpty())
    {
        ui->inputBrowsePath->setText(newPath);
    }
}



void MainWindow::on_beginCopyingButton_released()
{
    QMessageBox msgbox;
    msgbox.setWindowTitle("Begin copying");
    msgbox.setIcon(QMessageBox::Question);
    msgbox.setText(tr("Sure you want to start the copying process?"));
    msgbox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    if(msgbox.exec() != QMessageBox::Yes)
    {
        return;
    }

    CopyingDialog *mDialog = new CopyingDialog(0);
    mDialog->setInputDirectory(ui->inputBrowsePath->text());

    //Converts the list from a QList to QStringList
    QStringList selectedDrives;
    foreach(QListWidgetItem *drive, ui->drivesList->selectedItems())
    {
        selectedDrives.append(drive->text());
    }

    mDialog->setFormatDiskLabel(ui->formatedDiskLabel->text());
    mDialog->setFormatDrives(ui->formatDrive->isChecked());
    mDialog->setOverwritefiles(ui->overwriteFiles->isChecked());
    mDialog->setValidateData(ui->validateData->isChecked());
    mDialog->setOutputDirectories(selectedDrives);
    mDialog->setModal(true);
    mDialog->show();
    mDialog->beginCopying();
}

void MainWindow::on_refreshListButton_clicked()
{
    updateDrivesList();
}

void MainWindow::on_drivesList_clicked(const QModelIndex &index)
{
    ui->beginCopyingButton->setEnabled(true);
    if(ui->drivesList->selectedItems().count() <= 0 )
    {
        ui->beginCopyingButton->setEnabled(false);
    }
}

void MainWindow::on_formatDrive_clicked()
{
    //format [drive] [filesystem] [QuickFormat] [Force eject disk] [label]
    //format D: /FS:FAT32 /Q /X /V:DISK
}

void MainWindow::on_formatDrive_toggled(bool checked)
{
    ui->formatedDiskLabel->setVisible(checked);

    if(checked)
    {
        QMessageBox msgbox;
        msgbox.setWindowTitle(tr("WARNING"));
        msgbox.setIcon(QMessageBox::Warning);
        msgbox.setText(tr("Proceed with caution. If this option is active it will format ALL selected drives before copying the data. This will ease ALL data from the drives and CAN´T be undone!"));
        msgbox.exec();
    }
}

void MainWindow::on_viewOnlyMountedVolumes_clicked()
{
    updateDrivesList();
}
