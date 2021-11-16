#include <QCoreApplication>
#include "../scrcpy/adbprocess.h"
#include "../scrcpy/server.h"
#include "../scrcpy/device.h"
#include "../scrcpy/controller/action.h"

class myDevice : public Device
{
private:
    /* data */
public:
    myDevice(/* args */) = default;

    virtual void consumeFrame()
    {
        qInfo() << "onNewFrame";
        auto frame = getFrame();

        qInfo()<<frame->height;
        qInfo()<<frame->width;
        qInfo()<<frame->data[1]<<frame->data[2]<<frame->data[3];

    }
};

int main(int argc, char *argv[])
{
    QCoreApplication *QApp = new QCoreApplication(argc, argv);
    Adbprocess adbScrpy;
    Server mServer;
    myDevice device;

    qInfo() << mServer.startServer();

    adbScrpy.setAdbPatch("/usr/bin/adb");
    adbScrpy.setSerial("127.0.0.1:6997");

    device.setDeviceName("127.0.0.1:6997");
    mServer.pushDevice(&device);

    while (!adbScrpy.autoConnect()) //自动连接
    {
        qInfo() << "autoConnect";
    };

    return QApp->exec();
}