#ifndef COPYPROGRESS_H
#define COPYPROGRESS_H

#include <QWidget>
#include <QCryptographicHash>

namespace Ui {
class CopyProgress;
}

class CopyProgress : public QWidget
{
    Q_OBJECT

public:
    explicit CopyProgress(QWidget *parent = 0);
    ~CopyProgress();

public slots:
    void setProgressValue(int value);
    void setProgressMaximumValue(int value);
    void setHeaderLabel(QString text);
    void progressCompleted();

private slots:
    void on_cancelButton_clicked();
    void onProgressValueChanged();

signals:
    void cancelProgress();

private:
    Ui::CopyProgress *ui;


};

#endif // COPYPROGRESS_H
