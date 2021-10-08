#ifndef SERVER_H
#define SERVER_H

#include <QtCore>

class TcpServer;
class Device;
class VideoSocket;


class Server : public QObject
{
    Q_OBJECT
public:
    Server(QObject *parent = nullptr);
    ~Server();
    bool startServer(int port = 37321);
    void pushDevice(Device *device);
    void popDevice(QString deviceName);
private slots:
    void onNewConnect();

private:
    TcpServer *mServer = Q_NULLPTR;
    QMap<QString,QPointer<Device>> deviceList;
    bool readInfo(VideoSocket *videoSocket, QString &deviceName, QSize &size);
    QString LastDevName;
};

#endif