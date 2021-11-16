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
    myDevice(/* args */)
    {
    }
    ~myDevice()
    {
    }
    virtual void consumeFrame()
    {
        qInfo() << "onNewFrame";
        auto frame = getFrame();

    }
};

int main(int argc, char *argv[])
{
    QCoreApplication *QApp = new QCoreApplication(argc, argv);
    Adbprocess adbScrpy,autouimator;
    Server mServer;
    myDevice device;

    qInfo() << mServer.startServer();

    adbScrpy.setAdbPatch("/usr/bin/adb");
    adbScrpy.setSerial("127.0.0.1:6997");

    autouimator.setAdbPatch("/usr/bin/adb");
    autouimator.setSerial("127.0.0.1:6997");
    device.setDeviceName("127.0.0.1:6997");
    mServer.pushDevice(&device);

    // while (!adbScrpy.autoConnect()) //自动连接
    // {
    //     qInfo()<<"autoConnect";
    // };

    qInfo()<<autouimator.uiautomator();

    return QApp->exec();
}