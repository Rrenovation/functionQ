#functionQ
===========================

functionQ adb连接Android设备，获取显示图像和控制。不需要root权限。

### Ubuntu 环境依赖
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
### 简单应用: main.cpp
```cpp
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
    
    device->setDeviceName("127.0.0.1:5555");

    mServer->pushDevice(device);

    return QApp->exec();
}
````

## 参考
---
[1] [scrcpy](https://github.com/Genymobile/scrcpy)
[2] [QtScrcpy](https://github.com/barry-ran/QtScrcpy)
