#ifndef VIDEO_BUFFER_H
#define VIDEO_BUFFER_H
#include <QMutex>
#include <QWaitCondition>

#ifdef __cplusplus
extern "C"
{
#endif
#include "libavutil/pixfmt.h"
#ifdef __cplusplus
}
#endif

// forward declarations
typedef struct AVFrame AVFrame;

class VideoBuffer
{
public:
    VideoBuffer();
    virtual ~VideoBuffer();

    bool init(bool renderExpiredFrames = false);
    void deInit();
    void lock();
    void unLock();

    AVFrame *decodingFrame();
    // set the decoder frame as ready for rendering
    // this function locks m_mutex during its execution
    // returns true if the previous frame had been consumed
    void offerDecodedFrame(bool &previousFrameSkipped);

    // mark the rendering frame as consumed and return it
    // MUST be called with m_mutex locked!!!
    // the caller is expected to render the returned frame to some texture before
    // unlocking m_mutex
    const AVFrame *consumeRenderedFrame();

    const AVFrame *peekRenderedFrame();

    const AVFrame *consumeRenderedFrame(AVPixelFormat format);
    
    const AVFrame *ConvertPixelFormat(const AVFrame *srcFrame, AVPixelFormat format);

    const uint8_t *getFrameBuff();
    // wake up and avoid any blocking call
    void interrupt();

private:
    void swap();

private:
    AVFrame *m_decodingFrame = Q_NULLPTR;
    AVFrame *m_renderingframe = Q_NULLPTR;
    AVFrame *m_pFrameRGB = Q_NULLPTR;
    uint8_t *m_pFrameBuff = Q_NULLPTR;
    QMutex m_mutex;
    bool m_renderingFrameConsumed = true;

    bool m_renderExpiredFrames = false;
    QWaitCondition m_renderingFrameConsumedCond;

    // interrupted is not used if expired frames are not rendered
    // since offering a frame will never block
    bool m_interrupted = false;
};

#endif // VIDEO_BUFFER_H
