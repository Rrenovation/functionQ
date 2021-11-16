#functionQ
===========================

functionQ 一个远程控制安卓设备的通用库。通过adb连接Android设备，获取图像数据和对设备控制。

### library
```shell
    apt install libavformat-dev libavcodec-dev libswresample-dev libswscale-dev libavutil-dev libsdl1.2-dev cmake qt5-default -y
```
### compile
```shell
    git clone https://github.com/Rrenovation/functionQ.git
    cd functionQ
    mkdir build && cd build
    cmake ..
    make 
```
### exmaple: main.cpp
```cpp
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


        //get control obj       
        auto action = getAction();
        //simple control device 
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

    while (!adbScrpy.autoConnect()) //autoConnect
        ;

    qInfo()<<autouimator.uiautomator();

    return QApp->exec();
}

````
### start scrcpy-server on Android 
``` shell
    adb connect 127.0.0.1:6997
    adb -s 127.0.0.1:6997 push scrcpy-server /data/local/tmp/scrcpy-server
    adb -s 127.0.0.1:6997 reverse localabstract:scrcpy tcp:37321
    adb -s 127.0.0.1:6997 shell CLASSPATH=/data/local/tmp/scrcpy-server app_process / com.genymobile.scrcpy.Server 1.18 info 0 20000000 4 -1 false - true true 0 false false - - 127.0.0.1:6997
```

## 参考
---
[1] [scrcpy](https://github.com/Genymobile/scrcpy)
[2] [QtScrcpy](https://github.com/barry-ran/QtScrcpy)
