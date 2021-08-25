#include <QRandomGenerator>
#include "action.h"
#include "controlmsg.h"
#include "controller.h"

Action::Action()
{
    isSetCtl = false;
}
Action::~Action()
{
}

void Action::setCtl(Controller *ctl)
{
    this->ctl = ctl;
    isSetCtl = true;
}

void Action::sendText(QString text)
{
    if (!isSetCtl)
        return;
    ControlMsg *ctlmsg = new ControlMsg(ControlMsg::CMT_INJECT_TEXT);
    ctlmsg->setInjectTextMsgData(text);
    ctl->postControlEvent(ctlmsg);
}
void Action::sendTextEx(QString text)
{
    if (!isSetCtl)
        return;
    ControlMsg *ctlmsg = new ControlMsg(ControlMsg::CMT_SET_CLIPBOARD);
    ctlmsg->setSetClipboardMsgData(text,true);
    ctl->postControlEvent(ctlmsg);    
}


// msec单位毫秒
void Action::sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

int Action::random(int lowest, int highest)
{
    return QRandomGenerator::global()->bounded(lowest, highest);
}

void Action::deley()
{
    sleep(random(lowest, highest));
}

void Action::deley(int lowest, int highest)
{
    sleep(random(lowest, highest));
}

void Action::click(QRect position, quint64 id)
{
    if (!isSetCtl)
        return;
    clickHold(position, id);
    //    deley();
    clickRelease(position, id);
}

void Action::click(QRect position)
{
    click(position, 0);
}

void Action::clickHold(QRect position, quint64 id)
{
    if (!isSetCtl)
        return;
    ControlMsg *ctlmsgDown = new ControlMsg(ControlMsg::CMT_INJECT_TOUCH);
    float fd = random(lowest, highest);
    ctlmsgDown->setInjectTouchMsgData(id, AMOTION_EVENT_ACTION_DOWN, AMOTION_EVENT_BUTTON_PRIMARY, position, fd / 100);
    ctl->postControlEvent(ctlmsgDown);
}

void Action::clickRelease(QRect position, quint64 id)
{
    if (!isSetCtl)
        return;
    ControlMsg *ctlmsgRelsease = new ControlMsg(ControlMsg::CMT_INJECT_TOUCH);
    float fd = random(lowest, highest);
    ctlmsgRelsease->setInjectTouchMsgData(id, AMOTION_EVENT_ACTION_UP, AMOTION_EVENT_BUTTON_PRIMARY, position, fd / 100);
    ctl->postControlEvent(ctlmsgRelsease);
}

void Action::clickRelease(QRect position)
{
    clickRelease(position, 0);
}

void Action::fingerHold(QRect position, quint64 id)
{
    clickHold(position, id);
}

void Action::fingerMove(QRect position, quint64 id)
{
    if (!isSetCtl)
        return;
    ControlMsg *ctlmsg = new ControlMsg(ControlMsg::CMT_INJECT_TOUCH);
    float fd = random(lowest, highest);
    ctlmsg->setInjectTouchMsgData(id, AMOTION_EVENT_ACTION_MOVE, AMOTION_EVENT_BUTTON_PRIMARY, position, fd / 100);
    ctl->postControlEvent(ctlmsg);
}

void Action::fingerRelease(QRect position, quint64 id)
{
    clickRelease(position, 0);
}

void Action::fingerScroll(QRect positionStart, QRect positionStop, quint64 id)
{
    double k = kResult(positionStart.x(), positionStop.x(), positionStart.y(), positionStop.y());

    fingerHold(positionStart, id);
    if (positionStart.x() > positionStop.x())
    {
        for (int i = 1; i <= positionStart.x() - positionStop.x(); i += 5)
        {
            int startY = positionStart.y() - i * k + random(-10, 10);
            fingerMove(QRect(positionStart.x() - i, startY, positionStop.width(), positionStop.height()), id);
            sleep(scrollTime);
        }
    }
    else
    {
        for (int i = 1; i <= positionStop.x() - positionStart.x(); i += 5)
        {
            int startY = positionStart.y() + i * k + random(-10, 10);
            fingerMove(QRect(positionStart.x() + i, startY, positionStop.width(), positionStop.height()), id);
        }
        sleep(scrollTime);
    }
    fingerRelease(positionStop, id);
}

void Action::setScrollTime(int scrollTime)
{
    this->scrollTime = scrollTime;
}

void Action::fingerScroll(QRect positionStart, QRect positionStop)
{
    fingerScroll(positionStart, positionStop, 0);
}

void Action::postKeyCodeClickHold(AndroidKeycode keycode)
{
    ControlMsg *controlEventDown = new ControlMsg(ControlMsg::CMT_INJECT_KEYCODE);
    if (!controlEventDown)
    {
        return;
    }
    controlEventDown->setInjectKeycodeMsgData(AKEY_EVENT_ACTION_DOWN, keycode, 0, AMETA_NONE);
    ctl->postControlEvent(controlEventDown);
}

void Action::postKeyCodeClickRelease(AndroidKeycode keycode)
{
    ControlMsg *controlEventUp = new ControlMsg(ControlMsg::CMT_INJECT_KEYCODE);
    if (!controlEventUp)
    {
        return;
    }
    controlEventUp->setInjectKeycodeMsgData(AKEY_EVENT_ACTION_UP, keycode, 0, AMETA_NONE);
    ctl->postControlEvent(controlEventUp);
}

void Action::postKeyCodeClick(AndroidKeycode keycode)
{
    postKeyCodeClickHold(keycode);
    postKeyCodeClickRelease(keycode);
}

void Action::goBack()
{
    postKeyCodeClick(AKEYCODE_BACK);
}
void Action::goHome()
{
    postKeyCodeClick(AKEYCODE_HOME);
}
void Action::goMenu()
{
    postKeyCodeClick(AKEYCODE_MENU);
}
void Action::appSwitch()
{
    postKeyCodeClick(AKEYCODE_APP_SWITCH);
}
void Action::onPower()
{
    postKeyCodeClick(AKEYCODE_POWER);
}

void Action::onCopy()
{
    postKeyCodeClick(AKEYCODE_COPY);
}

void Action::onCut()
{
    postKeyCodeClick(AKEYCODE_CUT);
}

void Action::onClear()
{
    postKeyCodeClick(AKEYCODE_DEL);
}

void Action::onClear(int num)
{
    while (num--)
        postKeyCodeClick(AKEYCODE_DEL);
}

double Action::kResult(int x1, int x2, int y1, int y2)
{
    qInfo() << x1 << x2 << y1 << y2;
    if ((y1 - y2) == 0)
        return 0;
    return (y1 - y2) / (x1 - x2);
}