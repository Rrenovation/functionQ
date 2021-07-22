#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QtNetwork/qhostaddress.h>
#include "adbprocess.h"

QString Adbprocess::s_adbPath = "";

Adbprocess::Adbprocess(QObject *parent) : QProcess(parent)
{
    connectStep = UNCONNECT;
}

Adbprocess::~Adbprocess()
{
    if (isRuning())
    {
        close();
    }
}

const QString &Adbprocess::getAdbPath()
{
    if (s_adbPath.isEmpty())
    {
        if (s_adbPath.isEmpty())
        {
            s_adbPath = QCoreApplication::applicationDirPath() + "adb";
        }
        qInfo("adb path: %s", QDir(s_adbPath).absolutePath().toUtf8().data());
    }
    return s_adbPath;
}

bool Adbprocess::isRuning()
{
    if (QProcess::NotRunning == state())
    {
        return false;
    }
    else
    {
        return true;
    }
}

void Adbprocess::connectDevice(const QString &serial)
{

    QStringList ipInfo = serial.split(":");
    QHostAddress _Address;
    bool _isAddr = _Address.setAddress(ipInfo.at(0)); //timmed是去掉空格
    if (!_isAddr)
    {
        qInfo() << "_isAddr" << _isAddr;
        connectStep = WAITPUSH;
        return;
    }
    QStringList adbArgs;
    adbArgs << "connect";
    adbArgs << serial;
//    qInfo() << getAdbPath() << adbArgs.join(" ");
    start(getAdbPath(), adbArgs);
    waitForFinished();
    QString result(readAllStandardOutput());
//    qInfo() << result;
    if (result.contains("already connected", Qt::CaseSensitive))
    {
        qInfo() << "wait for push";
        connectStep = WAITPUSH;
    }
}

void Adbprocess::reverse(const QString &serial, const QString &deviceSocketName, quint16 localPort)
{
    QStringList adbArgs;
    adbArgs << "-s";
    adbArgs << serial;
    adbArgs << "reverse";
    adbArgs << QString("localabstract:%1").arg(deviceSocketName);
    adbArgs << QString("tcp:%1").arg(localPort);
    //qInfo() << getAdbPath() << adbArgs.join(" ");
    start(getAdbPath(), adbArgs);
    waitForFinished();
    QString result(readAllStandardOutput());
    // qInfo() << result;
    connectStep = WAITPROCESS;
}

void Adbprocess::push(const QString &serial, const QString &local, const QString &remote)
{
    QStringList adbArgs;
    adbArgs << "-s";
    adbArgs << serial;
    adbArgs << "push";
    adbArgs << local;
    adbArgs << remote;
//    qInfo() << getAdbPath() << adbArgs.join(" ");
    start(getAdbPath(), adbArgs);
    waitForFinished();
    QString result(readAllStandardOutput());
    qInfo() << result;
    if (result.contains("file pushed", Qt::CaseSensitive))
    {
        qInfo() << "wait for reverse";
        connectStep = WAITREVERSE;
    }
    else
    {
        connectStep = UNCONNECT;
    }
}

void Adbprocess::appprocess(const QString &serial)
{
    QStringList adbArgs;
    adbArgs << "-s";
    adbArgs << serial;
    adbArgs << "shell";
    adbArgs << "CLASSPATH=/data/local/tmp/lewone-server";
    adbArgs << "app_process";
    adbArgs << "/";
    adbArgs << "com.tencent.mtt.Server";
    adbArgs << "1.21";
    adbArgs << "info";
    adbArgs << "0";
    adbArgs << "20000000";
    adbArgs << "4";
    adbArgs << "-1";
    adbArgs << "false";
    adbArgs << "-";
    adbArgs << "true";
    adbArgs << "true";
    adbArgs << "0";
    adbArgs << "false";
    adbArgs << "false";
    adbArgs << "-";
    adbArgs << "-";
    adbArgs << serial;

    //qInfo() << getAdbPath() << adbArgs.join(" ");
    
    start(getAdbPath(), adbArgs);
    waitForReadyRead();
    QString result(readAllStandardOutput());
     qInfo() << result;
    if (result.contains("DesktopSetDevice:", Qt::CaseSensitive))
    {
        connectStep = CONNECTED;
    }
}

void Adbprocess::setStatus(CONNECTSTEP status)
{
    connectStep = status;
}

void Adbprocess::autoConnect(const QString &serial)
{
    // UNCONNECT,          //adb未连接
    // WAITPUSH,           //等待push lewone-server
    // WAITREVERSE,        //等待reverse
    // WAITPROCESS,        //等待app_process
    // CONNECTED,          //设备已连接

    switch (connectStep)
    {
    case UNCONNECT:
        connectDevice(serial);
        break;
    case WAITPUSH:
        push(serial, "lewone-server", "/data/local/tmp/lewone-server");
        break;
    case WAITREVERSE:
        reverse(serial, "lewone", 37321);
        break;
    case WAITPROCESS:
        appprocess(serial);
        break;
    case CONNECTED:
        qInfo() << serial << " connected " << QString(readAllStandardOutput());
        mserial = serial;
        break;
    default:
        break;
    }
}

void Adbprocess::startapplication(QString application) //adb shell am start -n 包名/类名
{
    QStringList adbArgs;
    adbArgs << "-s";
    adbArgs << mserial;
    adbArgs << "shell";
    adbArgs << "am";
    adbArgs << "start";
    adbArgs << "-n";
    adbArgs << application + QString("/") + application + QString(".MainActivity");
    qInfo() << getAdbPath() << adbArgs.join(" ");
    start(getAdbPath(), adbArgs);
    waitForReadyRead();
    QString result(readAllStandardOutput());
    qInfo() << result;
}

void Adbprocess::killapplication(QString application) //adb shell am force-stop
{
    QStringList adbArgs;
    adbArgs << "-s";
    adbArgs << mserial;
    adbArgs << "shell";
    adbArgs << "am";
    adbArgs << "force-stop";
    adbArgs << application;
    qInfo() << getAdbPath() << adbArgs.join(" ");
    start(getAdbPath(), adbArgs);
    waitForReadyRead();
    QString result(readAllStandardOutput());
    qInfo() << result;
}

void Adbprocess::setSerial(QString &serial)
{
    mserial = serial;
}