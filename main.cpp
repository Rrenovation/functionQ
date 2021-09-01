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
    Adbprocess adbScrpy,autouimator;
    Server mServer;
    myDevice device;

    mServer.startServer();

    adbScrpy.setAdbPatch("/home/qtubuntu/Android/Sdk/platform-tools/adb");
    adbScrpy.setSerial("12.168.1.47:5555");

    autouimator.setAdbPatch("/home/qtubuntu/Android/Sdk/platform-tools/adb");
    autouimator.setSerial("12.168.1.47:5555");

    device.setDeviceName("12.168.1.47:5555");
    mServer.pushDevice(&device);

    while (!adbScrpy.autoConnect()) //自动连接
        ;

    qInfo()<<autouimator.uiautomator();

    return QApp->exec();
}