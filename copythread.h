#ifndef COPYTHREAD_H
#define COPYTHREAD_H

#include <QObject>
#include <QThread>
#include <QtCore>
#include <QCryptographicHash>

class CopyThread : public QThread
{
    Q_OBJECT

public:
    explicit CopyThread();

    QString sourceFolder() const;
    void setSourceFolder(const QString sourceFolder);
    QString destinationFolder() const;
    void setDestinationFolder(const QString destinationFolder);
    bool overwritefiles() const;
    void setOverwritefiles(bool overwritefiles);
    bool validateData() const;
    void setValidateData(bool validateData);

protected:
    void run();

private:
    void copyFolder(QString srcFolder, QString destFolder);
    bool copyFile(QString srcFilename, QString destFilename, bool overwrite);

    QString _sourceFolder;
    QString _destinationFolder;

    int copiedFilesCount;
    bool Stop;
    bool _overwritefiles;
    bool _validateData;

    QByteArray fileChecksum(const QString &fileName,
                      QCryptographicHash::Algorithm hashAlgorithm);

public slots:
    void cancelProgress();

signals:
    void fileCopied(bool,QString);
    void statusText(QString);
    void filesCopied(int);
    void onFileValidated(bool,QString);
    void onFileCopied(bool,QString);
    void completed();
    void aborted();
};

#endif // COPYTHREAD_H
