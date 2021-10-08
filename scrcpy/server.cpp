#include "server.h"
#include "device.h"
#include "./decoder/tcpserver.h"
#include "./decoder/stream.h"
#include "./decoder/videosocket.h"
#include "./controller/controller.h"

#define DEVICE_NAME_FIELD_LENGTH 128

Server::Server(QObject *parent) : QObject(parent)
{
    if (!Stream::init())
    {
        qInfo() << "Stream Init fail !";
        return;
    }
}

Server::~Server()
{
    Stream::deInit();
    mServer->close();
    delete mServer;
}

bool Server::startServer(int port)
{
    if (mServer == Q_NULLPTR)
    {
        mServer = new TcpServer();
        if (mServer == Q_NULLPTR)
        {
            return false;
        }
    }
    if (!mServer->listen(QHostAddress::Any, port))
    {
        qInfo() << "server listen " << port << " fail!";
        delete mServer;
        mServer = Q_NULLPTR;
        return false;
    }
    connect(mServer, &TcpServer::newConnection, this, &Server::onNewConnect);
    return true;
}

void Server::onNewConnect()
{
    QTcpSocket *tmp = mServer->nextPendingConnection();
    if (dynamic_cast<VideoSocket *>(tmp))
    {
        QPointer<VideoSocket> videoSocket = dynamic_cast<VideoSocket *>(tmp);
        QString deviceName;
        QSize size(0, 0);
        if (videoSocket && videoSocket->isValid() && readInfo(videoSocket, deviceName, size))
        {
            if (deviceList.find(deviceName) != deviceList.end())
            {
                auto device = deviceList[deviceName];
                if (device != Q_NULLPTR)
                {
                    qInfo() << "New" << deviceName << "Width:" << size.width() << "Height:" << size.height() << "connected";
                    device->setVideoSocket(videoSocket);
                    LastDevName = deviceName;
                    return;
                }
            }
        }
        qInfo("The connection is not a scrcpy device or the device is not in the device list !");
        mServer->setIsVideoSocket(true);
        videoSocket->disconnect();
        videoSocket->close();
        videoSocket->deleteLater();
    }
    if (!mServer->getIsVideoSocket())
    {
        mServer->setIsVideoSocket(true);

        if (deviceList.find(LastDevName) != deviceList.end())
        {
            auto device = deviceList[LastDevName];
            if (device != Q_NULLPTR)
            {
                device->setDeviceSocket(tmp);
            }
        }
        LastDevName = "";
    }
}

bool Server::readInfo(VideoSocket *videoSocket, QString &deviceName, QSize &size)
{
    unsigned char buf[DEVICE_NAME_FIELD_LENGTH + 4];
    if (videoSocket->bytesAvailable() <= (DEVICE_NAME_FIELD_LENGTH + 4))
    {
        videoSocket->waitForReadyRead(300);
    }
    qint64 len = videoSocket->read((char *)buf, sizeof(buf));
    if (len < DEVICE_NAME_FIELD_LENGTH + 4)
    {
        qInfo("Could not retrieve device information");
        return false;
    }
    buf[DEVICE_NAME_FIELD_LENGTH - 1] = '\0'; // in case the client sends garbage
    // strcpy is safe here, since name contains at least DEVICE_NAME_FIELD_LENGTH bytes
    // and strlen(buf) < DEVICE_NAME_FIELD_LENGTH
    deviceName = (char *)buf;
    size.setWidth((buf[DEVICE_NAME_FIELD_LENGTH] << 8) | buf[DEVICE_NAME_FIELD_LENGTH + 1]);
    size.setHeight((buf[DEVICE_NAME_FIELD_LENGTH + 2] << 8) | buf[DEVICE_NAME_FIELD_LENGTH + 3]);
    return true;
}

void Server::pushDevice(Device *device)
{
    deviceList[device->getDeviceName()] = QPointer<Device>(device);
}
void Server::popDevice(QString deviceName)
{
    deviceList.remove(deviceName);
}