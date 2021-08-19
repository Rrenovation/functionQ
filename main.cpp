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
        //提取图像数据 (uint_8t*)
        auto frame = getFrameBuffer();
        //宽
        auto width = getRows();
        //高
        auto height = getCols();
        //获取控制对象        
        auto action = getAction();
        //返回桌面
        action->goHome();
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication *QApp = new QCoreApplication(argc, argv);

    Server mServer;
    mServer.startServer();
    myDevice device;
    device.setDeviceName("12.168.1.45:5555");
    mServer.pushDevice(&device);

    return QApp->exec();
}