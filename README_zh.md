# functionQ
[English](README.md)

用于远程控制Android设备的库。通过ADB连接Android设备，获取图像数据并控制设备。
```
*******************************************************************************************
*******************************************************************************************
**            *  --->adb approcess    *               *                  *               **
**            *  <--------h264        *               *   encode----->   *               **
** functionQ  *                       * scrcpy-server *                  *    android    **
**            *  control-signal--->   *               *   control---->   *               **
**            *  <--------clipboard   *               *                  *               ** 
*******************************************************************************************
*******************************************************************************************
```
### 依赖库
```shell
    apt install libavformat-dev libavcodec-dev libswresample-dev libswscale-dev libavutil-dev libsdl1.2-dev cmake qt5-default -y
```
### 编译
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
    virtual void consumeFrame() //视频帧回调函数
    {
        qInfo() << "onNewFrame";
        auto frame = getFrame(); //获取  RGB24 图像数据帧

        //获取控制对象（生命周期由functionQ管理）     
        auto action = getAction();
        //简单应用，返回桌面
        action->goHome();
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

````
### 在Android启动 scrcpy-server 
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
