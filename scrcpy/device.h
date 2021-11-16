#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QtCore>



class Decoder;
class Stream;
class VideoBuffer;
class VideoSocket;
class QTcpSocket;
class Controller;
class Action;

struct Frame
{
    const uint8_t *data = Q_NULLPTR;
    int width;
    int height;
};

class Device : public QObject
{
    Q_OBJECT
private:
    QString deviceName;
    QPointer<Decoder> decoder;
    QPointer<Stream> stream;
    QPointer<Controller> controller;
    std::shared_ptr<Action> action;
    std::shared_ptr<Frame> frame;

private slots:
    void consumeNewFrame();
signals:
    void newFrame();

public:
    Device(QObject *parent = nullptr);
    virtual ~Device();
    void setVideoSocket(VideoSocket *videoSocket);
    void setDeviceSocket(QTcpSocket *socket);
    void setDeviceName(QString deviceName);
    virtual void consumeFrame();
    const QString getDeviceName();
    std::shared_ptr<Action> getAction();
    std::shared_ptr<Frame> getFrame();
};

#endif