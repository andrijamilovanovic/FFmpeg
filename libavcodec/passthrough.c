

#include <limits.h>

#include "libavutil/avassert.h"
#include "libavutil/crc.h"
#include "libavutil/opt.h"
#include "avcodec.h"
#include "internal.h"
#include "get_bits.h"
#include "bytestream.h"
#include "golomb.h"
#include "flac.h"
#include "flacdata.h"
#include "flacdsp.h"
#include "thread.h"
#include "unary.h"


typedef struct NoneDecodeContext
{
    AVClass *class;
    
} NoneDecodeContext;

static av_cold int passthrough_decode_init(AVCodecContext *avctx)
{
    //NoneDecodeContext *s = avctx->priv_data;

    avctx->channels = 2;
    avctx->sample_fmt = avctx->codec->sample_fmts[0];
    avctx->bits_per_raw_sample = 8;
    
    av_log(avctx, AV_LOG_DEBUG, "===== Set channel count\n");

    return 0;
}

static int passthrough_decode_frame(AVCodecContext *avctx, void *data,
                             int *got_frame_ptr, AVPacket *avpkt)
{
    const uint8_t *src = avpkt->data;
    int buf_size       = avpkt->size;
    int ret;
    AVFrame *frame     = data;
    
    // set every time
    avctx->channels = 2;
    avctx->sample_fmt = avctx->codec->sample_fmts[0];
    
    av_log(avctx, AV_LOG_DEBUG, "===== decode frame %d\n", buf_size);
    
    frame->nb_samples = buf_size;
    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0)
        return ret;
    
    uint8_t *samples = frame->data[0];

    memcpy(samples, src, buf_size);

    *got_frame_ptr = 1;
    
    return buf_size;
}
static const AVOption options[] = {
{ NULL },
};

static const AVClass passthrough_decoder_class = {
    .class_name = "Passthrough decoder",
    .item_name  = av_default_item_name,
    .option     = options,
    .version    = LIBAVUTIL_VERSION_INT,
};
static const AVClass mpegh_decoder_class = {
    .class_name = "MPEG decoder",
    .item_name  = av_default_item_name,
    .option     = options,
    .version    = LIBAVUTIL_VERSION_INT,
};

AVCodec ff_passthrough_decoder = {
    .name           = "ac4",
    .type           = AVMEDIA_TYPE_AUDIO,
    .id             = AV_CODEC_ID_AC4,
    .priv_data_size = sizeof (NoneDecodeContext),
    .init           = passthrough_decode_init,
    .decode         = passthrough_decode_frame,
    .capabilities   = AV_CODEC_CAP_DR1,
    .long_name      = NULL_IF_CONFIG_SMALL("AC-4"),
    .sample_fmts    = (const enum AVSampleFormat[]) { AV_SAMPLE_FMT_U8P,
                                                     AV_SAMPLE_FMT_NONE },
    .priv_class     = &passthrough_decoder_class
};


AVCodec ff_mpegh_a_decoder = {
    .name           = "mpegh",
    .long_name      = NULL_IF_CONFIG_SMALL("MPEGH codec"),
    .type           = AVMEDIA_TYPE_AUDIO,
    .id             = AV_CODEC_ID_MPEGH_3D_AUDIO,
    .priv_data_size = sizeof(NoneDecodeContext),
    .init           = passthrough_decode_init,
    .decode         = passthrough_decode_frame,
    .capabilities   = AV_CODEC_CAP_CHANNEL_CONF | AV_CODEC_CAP_DR1, // | AV_CODEC_CAP_FRAME_THREADS,
    .sample_fmts    = (const enum AVSampleFormat[]) { AV_SAMPLE_FMT_U8P,
                                                      AV_SAMPLE_FMT_NONE },
    .priv_class     = &mpegh_decoder_class,
};
AVCodec ff_mpegh_m_decoder = {
    .name           = "mpegh",
    .long_name      = NULL_IF_CONFIG_SMALL("MPEGH codec"),
    .type           = AVMEDIA_TYPE_AUDIO,
    .id             = AV_CODEC_ID_MPEGH_3D_AUDIO_A,
    .priv_data_size = sizeof(NoneDecodeContext),
    .init           = passthrough_decode_init,
    .decode         = passthrough_decode_frame,
    .capabilities   = AV_CODEC_CAP_CHANNEL_CONF | AV_CODEC_CAP_DR1 , //| AV_CODEC_CAP_FRAME_THREADS,
    .sample_fmts    = (const enum AVSampleFormat[]) { AV_SAMPLE_FMT_U8P,
                                                      AV_SAMPLE_FMT_NONE },
    .priv_class     = &mpegh_decoder_class,
};
