#include <QCoreApplication>

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
        auto action = getAction();
        action->goHome();
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication *QApp = new QCoreApplication(argc, argv);

    auto mServer = new Server();
    mServer->startServer();

    auto device = new myDevice();
    device->setDeviceName("12.168.1.45:5555");

    mServer->pushDevice(device);

    return QApp->exec();
}