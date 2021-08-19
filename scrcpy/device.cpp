#include "device.h"

#include "./decoder/adbprocess.h"
#include "./decoder/decoder.h"
#include "./decoder/avframeconvert.h"
#include "./decoder/tcpserver.h"
#include "./decoder/stream.h"
#include "./decoder/videosocket.h"
#include "./decoder/videobuffer.h"
#include "./controller/controller.h"
#include "./controller/action.h"

Device::Device(/* args */)
{
    videoBuffer = new VideoBuffer();
    videoBuffer->init();
    decoder = new Decoder(videoBuffer);
    stream = new Stream();
    action = new Action();

    stream->setDecoder(decoder);

    connect(decoder, &Decoder::onNewFrame, this, &Device::consumeNewFrame, Qt::DirectConnection);
}

Device::~Device()
{

    if (videoSocket)
    {
        videoSocket->close();
    }
    stream->stopDecode();
    videoBuffer->deInit();
    delete action;
    delete controller;
    delete stream;
    delete decoder;
    delete videoBuffer;
}

QString Device::getDeviceName()
{
    return deviceName;
}

void Device::setVideoSocket(VideoSocket *videoSocket)
{
    stream->setVideoSocket(videoSocket);
    stream->startDecode();
    this->videoSocket = videoSocket;
}

void Device::setDeviceSocket(QTcpSocket *socket)
{
    if (controller == Q_NULLPTR)
    {
        controller = new Controller();
        action->setCtl(controller);
    }
    controller->setDeviceSocket(socket);
}

void Device::consumeNewFrame()
{
    videoBuffer->lock();
    frame = videoBuffer->consumeRenderedFrame(AV_PIX_FMT_RGB24);
    consumeFrame();
    qInfo() << frame->width << "--" << frame->height;
    videoBuffer->unLock();
}

void Device::consumeFrame()
{
    qInfo() << "consumed Device Frame";
}

void Device::setDeviceName(QString deviceName)
{
    this->deviceName = deviceName;
}

Action *Device::getAction()
{
    return action;
}

const uint8_t *Device::getFrameBuffer()
{
    return frame->data[0];
}

const int Device::getRows()
{
    return frame->width;
}

const int Device::getCols()
{
    return frame->height;
}