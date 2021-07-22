#include "functionQ.h"

functionQ::functionQ(int &argc, char **argv): QCoreApplication(argc,argv)
{
    if (!Stream::init())
    {
        qInfo() << "Stream Init fail !";
        return;
    }
}

functionQ::~functionQ()
{
    for (auto device : deviceInfoList)
    {
        device.videoSocket->close();
        device.stream->stopDecode();
        device.videoBuffer->deInit();
        delete device.stream;
        delete device.action;
        delete device.decoder;
        delete device.controller;
        delete device.adbprocess;
        delete device.videoBuffer;
        delete device.videoSocket;
        qInfo() << "close :" << device.deviceName;
        deviceInfoList.remove(device.deviceName);
    }
    mServer->close();
    Stream::deInit();
}

bool functionQ::loop()
{
    if (mServer == Q_NULLPTR)
    {
        mServer = new TcpServer();
    }
    if (!mServer->listen(QHostAddress::Any, listenPort))
    {
        qInfo() << "server listen " << listenPort << " fail!";
        return false;
    }
    connect(mServer, &TcpServer::newConnection, this, &functionQ::onNewConnect);
    return true;
}


void functionQ::run()
{

}

bool functionQ::readInfo(VideoSocket *videoSocket, QString &deviceName, QSize &size)
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

void functionQ::onNewConnect()
{
    QTcpSocket *tmp = mServer->nextPendingConnection();
    if (dynamic_cast<VideoSocket *>(tmp))
    {
        QPointer<VideoSocket> videoSocket = dynamic_cast<VideoSocket *>(tmp);
        QString deviceName;
        QSize size(0, 0);
        if (videoSocket && videoSocket->isValid() && readInfo(videoSocket, deviceName, size))
        {
            if (deviceInfoList.find(deviceName) == deviceInfoList.end())
            {
                VideoBuffer *mVideoBuffer = new VideoBuffer();
                mVideoBuffer->init(false);
                Decoder *mDecoder = new Decoder(mVideoBuffer, this);
                Stream *mStream = new Stream(this);
                mStream->setDecoder(mDecoder);
                mStream->setVideoSocket(videoSocket);
                mStream->startDecode();
                deviceInfoList[deviceName].stream = mStream;
                deviceInfoList[deviceName].decoder = mDecoder;
                deviceInfoList[deviceName].deviceName = deviceName;
                deviceInfoList[deviceName].videoSocket = videoSocket;
                deviceInfoList[deviceName].videoBuffer = mVideoBuffer;
                LastDevName = deviceName;
                qInfo() << "a new devcides connected " << deviceName << "height:" << size.height() << "width:" << size.width();
            }
            else
            {
                Stream *mStream = deviceInfoList[deviceName].stream;
                Decoder *mDecoder = deviceInfoList[deviceName].decoder;
                mStream->setDecoder(mDecoder);
                mStream->setVideoSocket(videoSocket);
                mStream->startDecode();
                LastDevName = deviceName;
                qInfo() << "a old devcides reconnected " << deviceName << "height:" << size.height() << "width:" << size.width();
            }
        }
        else
        {
            qInfo("connect not is a scrcpy devcies");
            mServer->setIsVideoSocket(true);
            videoSocket->disconnect();
            videoSocket->close();
            videoSocket->deleteLater();
        }
    }
    else
    {
        if (!mServer->getIsVideoSocket())
        {
            mServer->setIsVideoSocket(true);
            if (deviceInfoList[LastDevName].controller == Q_NULLPTR)
            {
                deviceInfoList[LastDevName].controller = new Controller(this);
            }
            deviceInfoList[LastDevName].controller->setDeviceSocket(tmp);
            qInfo() << "new devControl socket" << LastDevName;
            LastDevName = "";
        }
    }
}