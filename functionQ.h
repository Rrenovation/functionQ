#include <QtCore>
#include <QCoreApplication>
#include "./scrcpy/decoder/adbprocess.h"
#include "./scrcpy/decoder/decoder.h"
#include "./scrcpy/decoder/avframeconvert.h"
#include "./scrcpy/decoder/tcpserver.h"
#include "./scrcpy/decoder/stream.h"
#include "./scrcpy/decoder/videosocket.h"
#include "./scrcpy/decoder/videobuffer.h"
#include "./scrcpy/controller/controller.h"
#include "./scrcpy/controller/action.h"

struct DeviceInfo
{
    QString deviceName;
    Decoder *decoder = Q_NULLPTR;
    Stream *stream = Q_NULLPTR;
    VideoBuffer *videoBuffer = Q_NULLPTR;
    VideoSocket *videoSocket = Q_NULLPTR;
    Adbprocess *adbprocess = Q_NULLPTR;
    Controller *controller = Q_NULLPTR;
    Action *action = Q_NULLPTR;
};

class functionQ : public QCoreApplication
{
    Q_OBJECT
public:
    functionQ(int &argc, char **argv);
    ~functionQ();
    bool loop();
    void run();
private slots:
    void onNewConnect();

private:
    bool readInfo(VideoSocket *videoSocket, QString &deviceName, QSize &size);
    const quint16 DEVICE_NAME_FIELD_LENGTH = 128;
    const quint16 listenPort = 37321;
    QString LastDevName;
    TcpServer *mServer = Q_NULLPTR;
    QMap<QString, DeviceInfo> deviceInfoList;
};