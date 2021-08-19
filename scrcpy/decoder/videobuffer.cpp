#include "videobuffer.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#ifdef __cplusplus
}
#endif

VideoBuffer::VideoBuffer()
{
}

VideoBuffer::~VideoBuffer() {}

bool VideoBuffer::init(bool renderExpiredFrames)
{
    m_renderExpiredFrames = renderExpiredFrames;
    m_decodingFrame = av_frame_alloc();
    if (!m_decodingFrame)
    {
        goto error;
    }

    m_renderingframe = av_frame_alloc();
    if (!m_renderingframe)
    {
        goto error;
    }
    m_pFrameRGB = av_frame_alloc();
    if (!m_pFrameRGB)
    {
        goto error;
    }

    // there is initially no rendering frame, so consider it has already been
    // consumed
    m_renderingFrameConsumed = true;

    return true;

error:
    deInit();
    return false;
}

void VideoBuffer::deInit()
{
    if (m_decodingFrame)
    {
        av_frame_free(&m_decodingFrame);
        m_decodingFrame = Q_NULLPTR;
    }
    if (m_renderingframe)
    {
        av_frame_free(&m_renderingframe);
        m_renderingframe = Q_NULLPTR;
    }
    if (m_pFrameRGB)
    {
        av_frame_free(&m_pFrameRGB);
        m_pFrameRGB = Q_NULLPTR;

        if (m_pFrameBuff)
        {
            av_free(m_pFrameBuff);
            m_pFrameBuff = Q_NULLPTR;
        }
    }
}

void VideoBuffer::lock()
{
    m_mutex.lock();
}

void VideoBuffer::unLock()
{
    m_mutex.unlock();
}

AVFrame *VideoBuffer::decodingFrame()
{
    return m_decodingFrame;
}

void VideoBuffer::offerDecodedFrame(bool &previousFrameSkipped)
{
    m_mutex.lock();

    if (m_renderExpiredFrames)
    {
        // if m_renderExpiredFrames is enable, then the decoder must wait for the current
        // frame to be consumed
        while (!m_renderingFrameConsumed && !m_interrupted)
        {
            m_renderingFrameConsumedCond.wait(&m_mutex);
        }
    }

    swap();
    previousFrameSkipped = !m_renderingFrameConsumed;
    m_renderingFrameConsumed = false;
    m_mutex.unlock();
}

const AVFrame *VideoBuffer::consumeRenderedFrame()
{
    Q_ASSERT(!m_renderingFrameConsumed);
    m_renderingFrameConsumed = true;
    if (m_renderExpiredFrames)
    {
        // if m_renderExpiredFrames is enable, then notify the decoder the current frame is
        // consumed, so that it may push a new one
        m_renderingFrameConsumedCond.wakeOne();
    }
    return m_renderingframe;
}

const AVFrame *VideoBuffer::consumeRenderedFrame(AVPixelFormat format)
{
    ConvertPixelFormat(consumeRenderedFrame(), format);
    return m_pFrameRGB;
}

const uint8_t *VideoBuffer::getFrameBuff()
{
    return m_pFrameBuff;
}

const AVFrame *VideoBuffer::ConvertPixelFormat(const AVFrame *srcFrame, AVPixelFormat format)
{
    int width = srcFrame->width;
    int height = srcFrame->height;

    if (!m_pFrameBuff)
    {
        m_pFrameBuff =(uint8_t*) av_malloc(av_image_get_buffer_size(format, width, height, 1) * sizeof(uint8_t));
    }
    else
    {
        av_realloc(m_pFrameBuff, av_image_get_buffer_size(format, width, height, 1) * sizeof(uint8_t));
    }

    av_image_fill_arrays(m_pFrameRGB->data, m_pFrameRGB->linesize, m_pFrameBuff, (AVPixelFormat)format, width, height, 1);

    SwsContext *conversion = sws_getContext(width,
                                            height,
                                            (AVPixelFormat)srcFrame->format,
                                            width,
                                            height,
                                            (AVPixelFormat)format,
                                            SWS_FAST_BILINEAR,
                                            NULL,
                                            NULL,
                                            NULL);
    sws_scale(conversion, srcFrame->data, srcFrame->linesize, 0, height, m_pFrameRGB->data, m_pFrameRGB->linesize);
    sws_freeContext(conversion);

    m_pFrameRGB->format = format;
    m_pFrameRGB->width = width;
    m_pFrameRGB->height = height;

    return m_pFrameRGB;
}

const AVFrame *VideoBuffer::peekRenderedFrame()
{
    return m_renderingframe;
}

void VideoBuffer::interrupt()
{
    if (m_renderExpiredFrames)
    {
        m_mutex.lock();
        m_interrupted = true;
        m_mutex.unlock();
        // wake up blocking wait
        m_renderingFrameConsumedCond.wakeOne();
    }
}

void VideoBuffer::swap()
{
    AVFrame *tmp = m_decodingFrame;
    m_decodingFrame = m_renderingframe;
    m_renderingframe = tmp;
}
