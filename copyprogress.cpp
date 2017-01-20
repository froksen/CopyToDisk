#include "copyprogress.h"
#include "ui_copyprogress.h"
#include <QDebug>
#include <QCryptographicHash>

CopyProgress::CopyProgress(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CopyProgress)
{
    ui->setupUi(this);

    connect(ui->progressBar,SIGNAL(valueChanged(int)),this,SLOT(onProgressValueChanged()));
}

CopyProgress::~CopyProgress()
{
    delete ui;
}

void CopyProgress::setProgressValue(int value)
{
    qDebug() << "CopyProgress value set to" << value;
    ui->progressBar->setValue(value);
}

void CopyProgress::setProgressMaximumValue(int value)
{
     qDebug() << "CopyProgress Maximum set to" << value;
     ui->progressBar->setMaximum(value);
}

void CopyProgress::setHeaderLabel(QString text)
{
    ui->driveLetter->setText(text);
}

void CopyProgress::progressCompleted()
{
    ui->progress->setText(tr("Done"));
}


void CopyProgress::on_cancelButton_clicked()
{
    ui->progress->setText(tr("Stopping"));
    ui->cancelButton->setEnabled(false);
    emit cancelProgress();
}

void CopyProgress::onProgressValueChanged()
{
    QString part = QString::number(ui->progressBar->value());
    QString total = QString::number(ui->progressBar->maximum());
    QString text = QString("%1/%2").arg(part).arg(total);
    ui->progress->setText(text);
}


