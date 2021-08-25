#ifndef ACTION_H
#define ACTION_H
#include <QtCore>
#include <QRect>
#include "keycodes.h"

class Controller;
class Action 
{
public:
    Action();
    virtual ~Action();
    void setCtl(Controller *ctl);
    void sendText(QString text);
    void sendTextEx(QString text);

    void click(QRect position);
    void clickHold(QRect position);
    void clickRelease(QRect position);
    void fingerScroll(QRect positionStart, QRect positionStop);
    void fingerHold(QRect position);
    void fingerMove(QRect position);
    void fingerRelease(QRect position);

    void click(QRect position, quint64 id);
    void clickHold(QRect position, quint64 id);
    void clickRelease(QRect position, quint64 id);
    void fingerHold(QRect position, quint64 id);
    void fingerMove(QRect position, quint64 id);
    void fingerRelease(QRect position, quint64 id);
    void fingerScroll(QRect positionStart, QRect positionStop, quint64 id);

    void setScrollTime(int scrollTime);

    void onCopy();
    void onCut();
    void goBack();
    void goHome();
    void goMenu();
    void appSwitch();
    void onPower();
    void onClear();
    void onClear(int num);
    double kResult(int x1, int x2, int y1, int y2);
    void postKeyCodeClick(AndroidKeycode keycode);
    void postKeyCodeClickHold(AndroidKeycode keycode);
    void postKeyCodeClickRelease(AndroidKeycode keycode);
    void sleep(unsigned int msec);
    int random(int lowest, int highest);
    void deley();
    void deley(int lowest, int highest);

private:
    int lowest = 15;
    int highest = 30;
    int scrollTime = 0;
    int lowestScroll = 150;
    int highestScroll = 350;
    bool isSetCtl;
    Controller *ctl = nullptr;
};

#endif