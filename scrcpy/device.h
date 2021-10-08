#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QtCore>

// forward declarations

class Decoder;
class Stream;
class VideoBuffer;
class VideoSocket;
class QTcpSocket;
class Controller;
class Action;

struct Frame
{
    uint8_t *data = Q_NULLPTR;
    int width;
    int height;
};

class Device : public QObject
{
    Q_OBJECT
private:
    QString deviceName;
    Decoder *decoder = Q_NULLPTR;
    Stream *stream = Q_NULLPTR;
    VideoSocket *videoSocket = Q_NULLPTR;
    VideoBuffer *videoBuffer = Q_NULLPTR;
    Controller *controller = Q_NULLPTR;
    Action *action = Q_NULLPTR;
    Frame *frame = Q_NULLPTR;
    /* data */
private slots:
    void consumeNewFrame();

public:
    Device(/* args */);
    virtual ~Device();
    void setVideoSocket(VideoSocket *videoSocket);
    void setDeviceSocket(QTcpSocket *socket);
    void setDeviceName(QString deviceName);
    virtual void consumeFrame();
    QString getDeviceName();
    Action *getAction();
    const Frame *getFrame();
};

#endif