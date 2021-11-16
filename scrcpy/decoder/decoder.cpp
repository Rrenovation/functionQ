#include <QDebug>
#include "compat.h"
#include "decoder.h"
#include "videobuffer.h"

Decoder::Decoder(QObject *parent) : QObject(parent), m_vb(new VideoBuffer())
{
    m_vb->init();
}

Decoder::~Decoder() {
    m_vb->deInit();   
}

bool Decoder::open(const AVCodec *codec)
{
    // codec context
    m_codecCtx = avcodec_alloc_context3(codec);
    if (!m_codecCtx)
    {
        qCritical("Could not allocate decoder context");
        return false;
    }
    if (avcodec_open2(m_codecCtx, codec, NULL) < 0)
    {
        qCritical("Could not open H.264 codec");
        return false;
    }
    m_isCodecCtxOpen = true;
    return true;
}

void Decoder::close()
{
    if (!m_codecCtx)
    {
        return;
    }
    if (m_isCodecCtxOpen)
    {
        avcodec_close(m_codecCtx);
    }
    avcodec_free_context(&m_codecCtx);
}

bool Decoder::push(const AVPacket *packet)
{
    if (!m_codecCtx || !m_vb)
    {
        return false;
    }
    AVFrame *decodingFrame = m_vb->decodingFrame();
#ifdef QTSCRCPY_LAVF_HAS_NEW_ENCODING_DECODING_API
    int ret = -1;
    if ((ret = avcodec_send_packet(m_codecCtx, packet)) < 0)
    {
        char errorbuf[255] = {0};
        av_strerror(ret, errorbuf, 254);
        qCritical("Could not send video packet: %s", errorbuf);
        return false;
    }
    if (decodingFrame)
    {
        ret = avcodec_receive_frame(m_codecCtx, decodingFrame);
    }
    if (!ret)
    {
        // a frame was received
        pushFrame();
    }
    else if (ret != AVERROR(EAGAIN))
    {
        qCritical("Could not receive video frame: %d", ret);
        return false;
    }
#else
    int gotPicture = 0;
    int len = -1;
    if (decodingFrame)
    {
        len = avcodec_decode_video2(m_codecCtx, decodingFrame, &gotPicture, packet);
    }
    if (len < 0)
    {
        qCritical("Could not decode video packet: %d", len);
        return false;
    }
    if (gotPicture)
    {
        pushFrame();
    }
#endif
    return true;
}

void Decoder::interrupt()
{
    if (m_vb)
    {
        m_vb->interrupt();
    }
}

void Decoder::pushFrame()
{
    if (!m_vb)
    {
        return;
    }
    bool previousFrameSkipped = true;
    m_vb->offerDecodedFrame(previousFrameSkipped);
    if (previousFrameSkipped)
    {
        // the previous newFrame will consume this frame
        qInfo() << "the previous newFrame will consume this frame!";
        return;
    }
    emit onNewFrame();
}

AVFrame *Decoder::avFrameConvertPixelFormat(const AVFrame *src, AVPixelFormat dstFormat)
{
    int width = src->width;
    int height = src->height;
    AVFrame *dst = av_frame_alloc();

    static uint8_t *buff = (uint8_t *)av_malloc(av_image_get_buffer_size(dstFormat, width, height, 1) * sizeof(uint8_t));
    av_image_fill_arrays(dst->data, dst->linesize, buff, dstFormat, src->width, src->height, 1);
    //avpicture_fill((AVPicture*)dst, buff, AV_PIX_FMT_YUV420P,width, height);
    //allocPicture(dstFormat, width, height)
    SwsContext *conversion = sws_getContext(width,
                                            height,
                                            (AVPixelFormat)src->format,
                                            width,
                                            height,
                                            dstFormat,
                                            SWS_FAST_BILINEAR,
                                            NULL,
                                            NULL,
                                            NULL);
    sws_scale(conversion, src->data, src->linesize, 0, height, dst->data, dst->linesize);
    sws_freeContext(conversion);

    dst->format = dstFormat;
    dst->width = src->width;
    dst->height = src->height;

    return dst;
}

std::shared_ptr<VideoBuffer> Decoder::getVideoBuff() const
{
    return m_vb;
}