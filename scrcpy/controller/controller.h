#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <QObject>
#include <QPointer>

class QTcpSocket;
class ControlMsg;
class Controller : public QObject
{
    Q_OBJECT
public:
    Controller(QObject *parent = Q_NULLPTR);

    void setDeviceSocket(QTcpSocket *Socket);
    void postControlEvent(ControlMsg *controlMsg);
    void test(QRect rc);

protected:
    bool event(QEvent *event);

private:
    bool sendControl(const QByteArray &buffer);

private:
    QPointer<QTcpSocket> controlSocket;
};

#endif // CONTROLLER_H
