#ifndef ADBPROCESS_H
#define ADBPROCESS_H

#include <QProcess>
#include <QXmlStreamReader>
#include <QRegExp>

class Adbprocess : public QProcess
{
    Q_OBJECT

public:
    enum ConnectionStatus 
    {
        UNCONNECT,   //adb未连接
        WAITPUSH,    //等待push lewone-server
        WAITREVERSE, //等待reverse
        WAITPROCESS, //等待app_process
        CONNECTED,   //设备已连接
    };

    enum AutoMator
    {
        TEXT = 1,
        RESOURCEID,
        CLASS,
        PACKAGE,
    };

    explicit Adbprocess(QObject *parent = nullptr, QString Serial = "");
    virtual ~Adbprocess();

    void setSerial(QString serial);
    void setAdbPatch(QString ptach);

    bool autoConnect();
    bool connectDevice();
    void reverse(const QString &deviceSocketName, quint16 localPort);
    bool push(const QString &local, const QString &remote);
    bool appprocess(QString fps = "5");

    void startapplication(QString application); //adb shell am start -n 包名/类名 adb shell am start -n 包名/包名.MainActivity 
    void killapplication(QString application);  //adb shell am force-stop 包名

    bool isRuning();
    const QString getAdbPath();

    QVector<QStringList> uiautomator();
    QVector<QStringList> uiFind(QString text, AutoMator type = AutoMator::TEXT);
    QVector<QVector<int>> uiGetBounds(QString text, AutoMator type=AutoMator::TEXT);
private:
    QString adbPath;
    QString mSerial;
    ConnectionStatus connectStatus = UNCONNECT;
};

#endif // ADBPROCESS_H
