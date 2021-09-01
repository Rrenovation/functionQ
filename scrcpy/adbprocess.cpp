#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QtNetwork/qhostaddress.h>
#include "adbprocess.h"

Adbprocess::Adbprocess(QObject *parent, QString Serial) : QProcess(parent), mSerial(Serial)
{
    
}

Adbprocess::~Adbprocess()
{
    if (isRuning())
    {
        close();
    }
}

const QString Adbprocess::getAdbPath()
{
    return QCoreApplication::applicationDirPath() + "/adb";
}

void Adbprocess::setAdbPatch(QString patch)
{
    adbPath = patch;
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

bool Adbprocess::connectDevice()
{

    QStringList ipInfo = mSerial.split(":");
    QHostAddress _Address;

    bool _isAddr = _Address.setAddress(ipInfo.at(0));
    if (!_isAddr)
    {
        //usb device no need to connect
        return true;
    }

    QStringList adbArgs;
    adbArgs << "connect";
    adbArgs << mSerial;


    start(adbPath, adbArgs);
    waitForFinished();


    if (readAllStandardOutput().contains("already connected"))
    {
        return true;
    }
    return false;
}

void Adbprocess::reverse(const QString &deviceSocketName, quint16 localPort)
{
    QStringList adbArgs;

    adbArgs << "-s";
    adbArgs << mSerial;
    adbArgs << "reverse";
    adbArgs << QString("localabstract:%1").arg(deviceSocketName);
    adbArgs << QString("tcp:%1").arg(localPort);

    start(adbPath, adbArgs);
    waitForFinished();
}

bool Adbprocess::push(const QString &local, const QString &remote)
{
    QStringList adbArgs;

    adbArgs << "-s";
    adbArgs << mSerial;
    adbArgs << "push";
    adbArgs << local;
    adbArgs << remote;

    start(adbPath, adbArgs);
    waitForFinished();

    if (readAllStandardOutput().contains("file pushed"))
    {
        return true;
    }
    return false;
}

bool Adbprocess::appprocess()
{
    QStringList adbArgs;

    adbArgs << "-s";
    adbArgs << mSerial;
    adbArgs << "shell";
    adbArgs << "CLASSPATH=/data/local/tmp/scrcpy-server";
    adbArgs << "app_process";
    adbArgs << "/";
    adbArgs << "com.genymobile.scrcpy.Server";
    adbArgs << "1.18";
    adbArgs << "info";
    adbArgs << "0";
    adbArgs << "20000000";
    adbArgs << "4"; //FPS
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
    adbArgs << mSerial;

    start(adbPath, adbArgs);
    waitForReadyRead();

    if (readAllStandardOutput().contains("server"))
    {
        return true;
    }
    return false;
}

bool Adbprocess::autoConnect()
{
    // UNCONNECT,          //adb未连接
    // WAITPUSH,           //等待push scrcpy-server
    // WAITREVERSE,        //等待reverse
    // WAITPROCESS,        //等待app_process
    // CONNECTED,          //设备已连接
    switch (connectStatus)
    {
    case UNCONNECT:
        if (connectDevice())
        {
            connectStatus = WAITPUSH;
        }
        break;
    case WAITPUSH:
        if (push("scrcpy-server", "/data/local/tmp/scrcpy-server"))
        {
            connectStatus = WAITREVERSE;
        }
        break;
    case WAITREVERSE:
        reverse("scrcpy", 37321);
        connectStatus = WAITPROCESS;
        break;
    case WAITPROCESS:
        if (appprocess())
        {
            connectStatus = CONNECTED;
            return true;
        }
        connectStatus = UNCONNECT;
        break;

    default:
        connectStatus = UNCONNECT;
        break;
    }
    return false;
}

void Adbprocess::startapplication(QString application) //adb shell am start -n 包名/类名
{
    QStringList adbArgs;
    adbArgs << "-s";
    adbArgs << mSerial;
    adbArgs << "shell";
    adbArgs << "am";
    adbArgs << "start";
    adbArgs << "-n";
    adbArgs << application + QString("/") + application + QString(".MainActivity");
    qInfo() << adbPath << adbArgs.join(" ");
    start(adbPath, adbArgs);
    waitForReadyRead();
    QString result(readAllStandardOutput());
    qInfo() << result;
}

void Adbprocess::killapplication(QString application) //adb shell am force-stop
{
    QStringList adbArgs;
    adbArgs << "-s";
    adbArgs << mSerial;
    adbArgs << "shell";
    adbArgs << "am";
    adbArgs << "force-stop";
    adbArgs << application;
    qInfo() << adbPath << adbArgs.join(" ");
    start(adbPath, adbArgs);
    waitForReadyRead();
    QString result(readAllStandardOutput());
    qInfo() << result;
}

void Adbprocess::setSerial(QString serial)
{
    mSerial = serial;
}

QVector<QStringList> Adbprocess::uiautomator()
{
    QStringList adbArgs;

    adbArgs << "-s";
    adbArgs << mSerial;
    adbArgs << "shell";
    adbArgs << "uiautomator";
    adbArgs << "dump";
    adbArgs << "--compressed";
    adbArgs << "/data/local/tmp/ui.xml";

    start(adbPath, adbArgs);
    waitForFinished();

    adbArgs.clear();
    adbArgs << "-s";
    adbArgs << mSerial;
    adbArgs << "shell";
    adbArgs << "cat";
    adbArgs << "/data/local/tmp/ui.xml";

    start(adbPath, adbArgs);
    waitForFinished();

    QVector<QStringList> dumpUiList;

    QXmlStreamReader reader(readAllStandardOutput());

    // 解析 XML，直到结束
    while (!reader.atEnd())
    {
        // 读取下一个元素
        QXmlStreamReader::TokenType nType = reader.readNext();
        if (nType == QXmlStreamReader::StartElement)
        {
            QString strElementName = reader.name().toString();
            if (!strElementName.compare("node"))
            {
                auto butes = reader.attributes();
                QStringList nodeArgs;

                nodeArgs << butes.value("index").toString()
                         << butes.value("text").toString()
                         << butes.value("resource-id").toString()
                         << butes.value("class").toString()
                         << butes.value("package").toString()
                         << butes.value("content-desc").toString()
                         << butes.value("checked").toString()
                         << butes.value("clickable").toString()
                         << butes.value("enabled").toString()
                         << butes.value("focusable").toString()
                         << butes.value("focused").toString()
                         << butes.value("scrollable").toString()
                         << butes.value("long-clickable").toString()
                         << butes.value("password").toString()
                         << butes.value("selected").toString()
                         << butes.value("bounds").toString();

                dumpUiList.push_back(nodeArgs);
            }
        }
    }
    return dumpUiList;
}

QVector<QStringList> Adbprocess::uiFind(QString text, AutoMator type)
{
    QVector<QStringList> dumpUiList = uiautomator();
    QVector<QStringList> ret = {};

    for (int num = 0; num < dumpUiList.size(); num++)
    {
        auto info = dumpUiList[num];
        switch (type)
        {
        case AutoMator::TEXT:
            if (!info.at(AutoMator::TEXT).compare(text))
            {
                ret.push_back(info);
            }
            break;
        case AutoMator::RESOURCEID:
            if (!info.at(AutoMator::RESOURCEID).compare(text))
            {
                ret.push_back(info);
            }
            break;
        case AutoMator::CLASS:
            if (!info.at(AutoMator::CLASS).compare(text))
            {
                ret.push_back(info);
            }
            break;
        case AutoMator::PACKAGE:
            if (!info.at(AutoMator::PACKAGE).compare(text))
            {
                ret.push_back(info);
            }
            break;
        }
    }
    return ret;
}
QVector<QVector<int>> Adbprocess::uiGetBounds(QString text, AutoMator type)
{
    QVector<QVector<int>> ret = {};

    auto nodeList = uiFind(text, type);

    for (auto node : nodeList)
    {
        QVector<int> bounds;
        QRegExp rx("\\d+");

        /* 遍历查找匹配。 */
        int pos(0);
        while ((pos = rx.indexIn(node.at(15), pos)) != -1)
        {
            bounds.push_back(rx.cap(0).toInt());
            pos += rx.matchedLength();
        }
        ret.push_back(bounds);
    }

    return ret;
}