#ifndef ADBPROCESS_H
#define ADBPROCESS_H

#include <QProcess>

class Adbprocess : public QProcess
{
    Q_OBJECT

public:
    enum CONNECTSTEP
    {
        UNCONNECT,   //adb未连接
        WAITPUSH,    //等待push lewone-server
        WAITREVERSE, //等待reverse
        WAITPROCESS, //等待app_process
        CONNECTED,   //设备已连接
    };

    explicit Adbprocess(QObject *parent = nullptr);
    virtual ~Adbprocess();
    void setSerial(QString &serial);
    void autoConnect(const QString &serial);
    void connectDevice(const QString &serial);
    void reverse(const QString &serial, const QString &deviceSocketName, quint16 localPort);
    void push(const QString &serial, const QString &local, const QString &remote);
    void appprocess(const QString &serial);
    void startapplication(QString application); //adb shell am start -n 包名/类名 adb shell am start -n 包名/包名.MainActivity 
    void killapplication(QString application);  //adb shell am force-stop 包名
    void setStatus(CONNECTSTEP status);
    bool isRuning();
    static const QString &getAdbPath();

private:
    static QString s_adbPath;
    QString mserial;
    CONNECTSTEP connectStep;
};

#endif // ADBPROCESS_H
