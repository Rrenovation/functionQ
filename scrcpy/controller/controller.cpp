#include <QCoreApplication>
#include <QTcpSocket>
#include "controller.h"
#include "controlmsg.h"

Controller::Controller(QObject *parent)
    : QObject(parent)
{
}

void Controller::setDeviceSocket(QTcpSocket *cSocket)
{
    controlSocket = cSocket;
}

void Controller::postControlEvent(ControlMsg *controlMsg)
{
    if (controlMsg)
    {
        QCoreApplication::postEvent(this, controlMsg);
    }
}

void Controller::test(QRect rc)
{
    // ControlMsg* textEvent = new ControlMsg(ControlMsg::CMT_INJECT_TEXT);
    // textEvent->setInjectTextMsgData(QString("123456"));
    // postControlEvent(textEvent);

    ControlMsg *controlMsg = new ControlMsg(ControlMsg::CMT_INJECT_TOUCH);
    controlMsg->setInjectTouchMsgData(static_cast<quint64>(POINTER_ID_MOUSE), AMOTION_EVENT_ACTION_DOWN, AMOTION_EVENT_BUTTON_PRIMARY, rc, 1.0f);
    postControlEvent(controlMsg);

    ControlMsg *controlMsgrelease = new ControlMsg(ControlMsg::CMT_INJECT_TOUCH);
    controlMsgrelease->setInjectTouchMsgData(static_cast<quint64>(POINTER_ID_MOUSE), AMOTION_EVENT_ACTION_UP, AMOTION_EVENT_BUTTON_PRIMARY, rc, 1.0f);
    postControlEvent(controlMsgrelease);
}

bool Controller::event(QEvent *event)
{
    if (event && static_cast<ControlMsg::Type>(event->type()) == ControlMsg::Control)
    {
        ControlMsg *controlEvent = dynamic_cast<ControlMsg *>(event);
        if (controlEvent)
        {
            sendControl(controlEvent->serializeData());
        }
        return true;
    }
    return QObject::event(event);
}

bool Controller::sendControl(const QByteArray &buffer)
{
    if (buffer.isEmpty())
    {
        return false;
    }
    qint32 len = 0;
    if (controlSocket)
    {
        len = controlSocket->write(buffer.data(), buffer.length());
        //qInfo()<<"send control len :"<<len <<"buf:"<<buffer.toHex();
    }
    return len == buffer.length() ? true : false;
}
