# functionQ
[中文](README_zh.md)

A library for remote control of Android devices. Connect Android device through ADB to obtain image data and control the device.
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
        auto frame = getFrame(); //get the RGB24 Video Buffer

        //get control obj       
        auto action = getAction();
        //simple control device 
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

    while (!adbScrpy.autoConnect()) 
    {
        qInfo() << "autoConnect";
    };

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

## Thinks
---
[1] [scrcpy](https://github.com/Genymobile/scrcpy)
[2] [QtScrcpy](https://github.com/barry-ran/QtScrcpy)
