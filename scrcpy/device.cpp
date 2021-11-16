#include "device.h"

#include "./decoder/decoder.h"
#include "./decoder/avframeconvert.h"
#include "./decoder/tcpserver.h"
#include "./decoder/stream.h"
#include "./decoder/videosocket.h"
#include "./decoder/videobuffer.h"
#include "./controller/controller.h"
#include "./controller/action.h"

Device::Device(QObject *parent) : QObject(parent),
                                  decoder(new Decoder()), stream(new Stream()),
                                  controller(new Controller()), action(new Action),
                                  frame(new Frame())
{

    connect(decoder, &Decoder::onNewFrame, this, &Device::consumeNewFrame, Qt::DirectConnection);
}

Device::~Device()
{
    stream->stopDecode();

}

const QString Device::getDeviceName()
{
    return deviceName;
}

void Device::setVideoSocket(VideoSocket *videoSocket)
{
    stream->setDecoder(decoder);
    stream->setVideoSocket(videoSocket);
    stream->startDecode();
}

void Device::setDeviceSocket(QTcpSocket *socket)
{
    action->setCtl(controller);
    controller->setDeviceSocket(socket);
}

void Device::consumeNewFrame()
{
    auto videoBuffer = decoder->getVideoBuff();
    videoBuffer->lock();
    auto AVFrame = videoBuffer->consumeRenderedFrame(AV_PIX_FMT_RGB24);
    frame->data = AVFrame->data[0];
    frame->height = AVFrame->height;
    frame->width = AVFrame->width;
    consumeFrame();
    emit newFrame();
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

std::shared_ptr<Action> Device::getAction()
{
    return action;
}

std::shared_ptr<Frame> Device::getFrame()
{
    return frame;
}
