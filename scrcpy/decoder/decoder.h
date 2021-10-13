#ifndef DECODER_H
#define DECODER_H
#include <QObject>

#ifdef __cplusplus
extern "C"
{
#endif
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/pixfmt.h"
#include "libavutil/imgutils.h"
#ifdef __cplusplus
}
#endif

class VideoBuffer;
class Decoder : public QObject
{
    Q_OBJECT
public:
    Decoder(VideoBuffer *vb, QObject *parent = Q_NULLPTR);
    virtual ~Decoder();

    bool open(const AVCodec *codec);
    void close();
    bool push(const AVPacket *packet);
    void interrupt();
signals:
    void onNewFrame();
protected:
    AVFrame *avFrameConvertPixelFormat(const AVFrame *src, AVPixelFormat dstFormat);
    void pushFrame();

private:
    VideoBuffer *m_vb = Q_NULLPTR;
    AVCodecContext *m_codecCtx = Q_NULLPTR;
    uint8_t *p_global_bgr_buffer = Q_NULLPTR;
    bool m_isCodecCtxOpen = false;
};

#endif // DECODER_H
