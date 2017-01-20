#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <copyingdialog.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_inputBrowseButton_clicked();
    void on_beginCopyingButton_released();
    void on_refreshListButton_clicked();
    void on_drivesList_clicked(const QModelIndex &index);
    void on_formatDrive_clicked();
    void on_formatDrive_toggled(bool checked);
    void upstartWarning();

    void on_viewOnlyMountedVolumes_clicked();

private:
    Ui::MainWindow *ui;

    void updateDrivesList();
};

#endif // MAINWINDOW_H
