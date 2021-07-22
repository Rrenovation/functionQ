#ifndef CONTROLMSG_H
#define CONTROLMSG_H

#include <QBuffer>
#include <QRect>
#include <QString>

#include "input.h"
#include "keycodes.h"
#include "qscrcpyevent.h"

#define CONTROL_MSG_MAX_SIZE (1 << 18) // 256k

#define CONTROL_MSG_INJECT_TEXT_MAX_LENGTH 300
// type: 1 byte; paste flag: 1 byte; length: 4 bytes
#define CONTROL_MSG_CLIPBOARD_TEXT_MAX_LENGTH \
    (CONTROL_MSG_MAX_SIZE - 6)

#define POINTER_ID_MOUSE static_cast<quint64>(-1)

// ControlMsg
class ControlMsg : public QScrcpyEvent
{
public:
    enum ControlMsgType
    {
        CMT_NULL = -1,
        CMT_INJECT_KEYCODE = 0,
        CMT_INJECT_TEXT,                 //输入内容
        CMT_INJECT_TOUCH,                //触摸
        CMT_INJECT_SCROLL,               //鼠标滑轮滚动
        CMT_BACK_OR_SCREEN_ON,           //关闭或者打开电源
        CMT_EXPAND_NOTIFICATION_PANEL,   //打开通知面板
        CMT_COLLAPSE_NOTIFICATION_PANEL, //关闭通知面板
        CMT_GET_CLIPBOARD,               //获取剪切板内容
        CMT_SET_CLIPBOARD,               //设置剪切板内容
        CMT_SET_SCREEN_POWER_MODE
    };

    enum ScreenPowerMode
    {
        // see <https://android.googlesource.com/platform/frameworks/base.git/+/pie-release-2/core/java/android/view/SurfaceControl.java#305>
        SPM_OFF = 0,
        SPM_NORMAL = 2,
    };

    ControlMsg(ControlMsgType controlMsgType);
    virtual ~ControlMsg();

    void setInjectKeycodeMsgData(AndroidKeyeventAction action, AndroidKeycode keycode, quint32 repeat, AndroidMetastate metastate);
    void setInjectTextMsgData(QString &text);

    /*  1.id 代表一个触摸点，最多支持10个触摸点[0,9]
        2.action 只能是AMOTION_EVENT_ACTION_DOWN，AMOTION_EVENT_ACTION_UP，AMOTION_EVENT_ACTION_MOVE
        3 AndroidMotioneventButtons AMOTION_EVENT_BUTTON_PRIMARY
        4.position action动作对应的位置
        5.按压位置
        6.压力大小
    */
    void setInjectTouchMsgData(quint64 id, AndroidMotioneventAction action, AndroidMotioneventButtons buttons, QRect position, float pressure);
    void setInjectScrollMsgData(QRect position, qint32 hScroll, qint32 vScroll);
    void setSetClipboardMsgData(QString &text, bool paste);
    void setSetScreenPowerModeData(ControlMsg::ScreenPowerMode mode);

    QByteArray serializeData();

private:
    void writePosition(QBuffer &buffer, const QRect &value);
    quint16 toFixedPoint16(float f);

private:
    struct ControlMsgData
    {
        ControlMsgType type = CMT_NULL;
        union
        {
            struct
            {
                AndroidKeyeventAction action;
                AndroidKeycode keycode;
                quint32 repeat;
                AndroidMetastate metastate;
            } injectKeycode;
            struct
            {
                char *text = Q_NULLPTR;
            } injectText;
            struct
            {
                quint64 id;
                AndroidMotioneventAction action;
                AndroidMotioneventButtons buttons;
                QRect position;
                float pressure;
            } injectTouch;
            struct
            {
                QRect position;
                qint32 hScroll;
                qint32 vScroll;
            } injectScroll;
            struct
            {
                char *text = Q_NULLPTR;
                bool paste = true;
            } setClipboard;
            struct
            {
                ScreenPowerMode mode;
            } setScreenPowerMode;
        };

        ControlMsgData() {}
        ~ControlMsgData() {}
    };

    ControlMsgData m_data;
};

#endif // CONTROLMSG_H
