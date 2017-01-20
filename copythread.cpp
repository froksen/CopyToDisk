#include "copythread.h"
#include <QtCore>
#include <QDebug>
#include <QDir>

CopyThread::CopyThread()
{
    copiedFilesCount = 0;
    this->Stop = false;
}

void CopyThread::run()
{
    qDebug() << "Copying start";
    QMutex mutex;
    mutex.lock();
    copyFolder(sourceFolder(),destinationFolder());
    mutex.unlock();
    qDebug() << "Copying completed";
    emit completed();
}

void CopyThread::copyFolder(QString srcFolder, QString destFolder)
{
    //Checks if source exists
    QDir sourceDir(srcFolder);
    if(!sourceDir.exists())
    {
        qDebug() << "Source directory does not exists!";
        return;
    }

    //Checks if destanation folder exists. If not then it will be created.
    QDir destDir(destFolder);
    if(!destDir.exists())
    {
        destDir.mkdir(destFolder);
        qDebug() << "Destanation directory does not exists. Creating it now";
    }

    //Finds all files in sourcedir
    QStringList files = sourceDir.entryList(QDir::Files);
    if(files.count()<=0)
    {
        qDebug() << "No files found in" << srcFolder;
    }
    else
    {
        qDebug() << "Files" << files.count() << "found in" << srcFolder;
    }

    //Loops, and copies the files
    foreach(QString file, files)
    {
        if(Stop)
        {
            qDebug() << "ABORTED";
            break;
            emit aborted();
        }

        QString src = QDir::toNativeSeparators(srcFolder + "/" + file);
        QString dest = QDir::toNativeSeparators(destFolder + "/" + file);

        emit statusText(tr("Copying to %1").arg(dest));


        //The validate process
        QByteArray srcChecksum;
        if(validateData())
        {
            //Checksum of source file.
            srcChecksum = fileChecksum(src,QCryptographicHash::Sha1);
        }

        //COPY
        if(copyFile(src,dest,overwritefiles()))
        {
            qDebug() << "Validate data:" << validateData();
            if(validateData())
            {
                QByteArray destChecksum = fileChecksum(dest,QCryptographicHash::Sha1); //Checksum of the copied file.

                if(destChecksum == srcChecksum)
                {
                    emit onFileValidated(true,dest); //If valid
                }
                else
                {
                    emit onFileValidated(false,dest); // if not valid
                }
            }
            emit onFileCopied(true,dest); //If file is copied successfully
        }
        else
        {
            emit onFileCopied(false,dest); //If file is copied UNsuccessfully
        }

        //How many files that are copied, successful or failed.
        copiedFilesCount++;
        qDebug() << "CP Files Copied: " << copiedFilesCount;
        emit filesCopied(copiedFilesCount);
    }

    //Clears the files-list
    files.clear();

    //Finds alle subDirs
    QStringList subDirs = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    foreach(QString subDir, subDirs)
    {
        if(Stop)
        {
            break;
            emit aborted();
        }

        QString src = QDir::toNativeSeparators(srcFolder + "/" + subDir);
        QString dest = QDir::toNativeSeparators(destFolder + "/" + subDir);
        qDebug() << "CP: Entering folder" << subDir;
        copyFolder(src,dest);
    }
}

bool CopyThread::copyFile(QString srcFilename, QString destFilename, bool overwrite = false)
{
    QFile destFile(destFilename);
    QFile srcFile(srcFilename);

    qDebug() << "File overwrite:" << overwrite;
    if(overwrite)
    {
        if(destFile.remove())
        {
            qDebug() << "File removed";
        }
        else
        {
            return false;
            qDebug() << "File could not be removed";
        }

    }

    qDebug() << "File copy from" << srcFilename << "to" << destFilename;
    if(srcFile.copy(destFilename))
    {
        qDebug() << "File copied";
        return true;
    }

    qDebug() << "File could not be copied";
    return false;
}

bool CopyThread::validateData() const
{
    return _validateData;
}

void CopyThread::setValidateData(bool validateData)
{
    _validateData = validateData;
}

bool CopyThread::overwritefiles() const
{
    return _overwritefiles;
}

void CopyThread::setOverwritefiles(bool overwritefiles)
{
    _overwritefiles = overwritefiles;
}

void CopyThread::cancelProgress()
{
    this->Stop = true;
}

QString CopyThread::destinationFolder() const
{
    return _destinationFolder;
}

void CopyThread::setDestinationFolder(const QString destinationFolder)
{
    qDebug() << "Destination folder set to" << destinationFolder;
    _destinationFolder = destinationFolder;
}

QString CopyThread::sourceFolder() const
{
    return _sourceFolder;
}

void CopyThread::setSourceFolder(const QString sourceFolder)
{
    qDebug() << "Source folder set to" << sourceFolder;
    _sourceFolder = sourceFolder;
}

//Method from: http://stackoverflow.com/questions/16383392/how-to-get-the-sha-1-md5-checksum-of-a-file-with-qt
QByteArray CopyThread::fileChecksum(const QString &fileName,
                        QCryptographicHash::Algorithm hashAlgorithm)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&f)) {
            return hash.result();
        }
    }
    return QByteArray();
}
