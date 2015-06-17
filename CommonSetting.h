#ifndef COMMONSETTING_H
#define COMMONSETTING_H
#include <QObject>
#include <QTextCodec>
#include <QFile>
#include <QSettings>
#include <QEventLoop>
#include <QTime>
#include <QTimer>
#include <QCoreApplication>
#include <QThread>
#include <QAbstractSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QStringList>

class CommonSetting : public QObject
{
public:
    CommonSetting();
    ~CommonSetting();

    //设置编码为UTF8
    static void SetUTF8Code()
    {
        QTextCodec *codec=
                QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForLocale(codec);
        QTextCodec::setCodecForCStrings(codec);
        QTextCodec::setCodecForTr(codec);
    }

    //读取文件内容
    static QString ReadFile(const QString &fileName)
    {
        QFile file(fileName);
        QByteArray fileContent;
        if (!file.open(QIODevice::ReadOnly)){
            fileContent = "";
        }else{
            fileContent = file.readAll();
        }
        file.close();
        return QString(fileContent);
    }

    //写数据到文件
    static void WriteFile(QString fileName,const QString data)
    {
        QFile file(fileName);
        if(!file.open(QFile::WriteOnly |
                      QFile::Truncate)){
        }else{
            file.write(data.toLocal8Bit().data());
            file.close();
        }
    }

    //QSetting应用
    static void WriteSettings(const QString &ConfigFile,
                              const QString &key,
                              const QString value)
    {

        QSettings settings(ConfigFile,QSettings::IniFormat);
        settings.setValue(key,value);
    }

    static void Sleep(quint16 msec)
    {
        QTime dieTime = QTime::currentTime().addMSecs(msec);
        while(QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
};
#endif // COMMONSETTING_H
