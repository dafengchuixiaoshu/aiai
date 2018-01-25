/*****************************************************************************
 * IJKVideoToolBox.m
 *****************************************************************************
 *
 * copyright (c) 2014 Zhou Quan <zhouqicy@gmail.com>
 *
 * This file is part of ijkPlayer.
 *
 * ijkPlayer is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * ijkPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with ijkPlayer; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "IJKVideoToolBox.h"
#include "ijksdl_vout_overlay_videotoolbox.h"
#include "ffpipeline_ios.h"
#import <CoreMedia/CMSampleBuffer.h>
#import <CoreFoundation/CoreFoundation.h>
#import <CoreVideo/CVHostTime.h>
#include <mach/mach_time.h>
#include "libavformat/avc.h"
#include "ijksdl_vout_ios_gles2.h"
#include "h264_sps_parser.h"

#define IJK_VTB_FCC_AVC    SDL_FOURCC('C', 'c', 'v', 'a')
#define IJK_VTB_FCC_ESD    SDL_FOURCC('s', 'd', 's', 'e')
#define IJK_VTB_FCC_AVC1   SDL_FOURCC('1', 'c', 'v', 'a')


static double GetSystemTime()
{
    return ((int64_t)CVGetCurrentHostTime() * 1000.0) / ((int64_t)CVGetHostClockFrequency());
}

static void SortQueuePop(VideoToolBoxContext* context)
{
    if (!context->m_sort_queue || context->m_queue_depth == 0) {
        return;
    }
    pthread_mutex_lock(&context->m_queue_mutex);
    volatile sort_queue *top_frame = context->m_sort_queue;
    context->m_sort_queue = context->m_sort_queue->nextframe;
    context->m_queue_depth--;
    pthread_mutex_unlock(&context->m_queue_mutex);
    CVBufferRelease(top_frame->pixel_buffer_ref);
    free((void*)top_frame);
}

static void CFDictionarySetSInt32(CFMutableDictionaryRef dictionary, CFStringRef key, SInt32 numberSInt32)
{
    CFNumberRef number;
    number = CFNumberCreate(NULL, kCFNumberSInt32Type, &numberSInt32);
    CFDictionarySetValue(dictionary, key, number);
    CFRelease(number);
}


static CMSampleBufferRef CreateSampleBufferFrom(CMFormatDescriptionRef fmt_desc, void *demux_buff, size_t demux_size)
{
    OSStatus status;
    CMBlockBufferRef newBBufOut = NULL;
    CMSampleBufferRef sBufOut = NULL;

    status = CMBlockBufferCreateWithMemoryBlock(
                                                NULL,
                                                demux_buff,
                                                demux_size,
                                                kCFAllocatorNull,
                                                NULL,
                                                0,
                                                demux_size,
                                                FALSE,
                                                &newBBufOut);

    if (!status) {
        status = CMSampleBufferCreate(
                                      NULL,
                                      newBBufOut,
                                      TRUE,
                                      0,
                                      0,
                                      fmt_desc,
                                      1,
                                      0,
                                      NULL,
                                      0,
                                      NULL,
                                      &sBufOut);
    }

    CFRelease(newBBufOut);
    if (status == 0) {
        return sBufOut;
    } else {
        return NULL;
    }
}




static bool GetVTBPicture(VideoToolBoxContext* context, VTBPicture* pVTBPicture)
{
    *pVTBPicture = context->m_videobuffer;

    if (context->m_sort_queue == NULL) {
        return false;
    }
    pthread_mutex_lock(&context->m_queue_mutex);

    pVTBPicture->dts             = context->m_sort_queue->dts;
    pVTBPicture->pts             = context->m_sort_queue->pts;
    pVTBPicture->width           = context->m_sort_queue->width;
    pVTBPicture->height          = context->m_sort_queue->height;
    pVTBPicture->cvBufferRef     = CVBufferRetain(context->m_sort_queue->pixel_buffer_ref);

    pthread_mutex_unlock(&context->m_queue_mutex);

    return true;
}

static void vtb_free_picture(Frame *vp)
{
    if (vp->bmp) {
        SDL_VoutFreeYUVOverlay(vp->bmp);
        vp->bmp = NULL;
    }
}

static void vtb_alloc_picture(FFPlayer *ffp)
{
    VideoState *is  = ffp->is;
    Frame *vp       = &is->pictq.queue[is->pictq.windex];
    vtb_free_picture(vp);
    vp->bmp = SDL_Vout_CreateOverlay(vp->width, vp->height, SDL_FCC_NV12, ffp->vout);
    if (!vp->bmp) {
        av_log(NULL, AV_LOG_FATAL,
               "Error: can't alloc nv12 overlay \n");
        vtb_free_picture(vp);
    }
    SDL_LockMutex(is->pictq.mutex);
    vp->allocated = 1;
    SDL_CondSignal(is->pictq.cond);
    SDL_UnlockMutex(is->pictq.mutex);
}

static int vtb_queue_picture(
                             FFPlayer*       ffp,
                             VTBPicture*     picture,
                             double          pts,
                             double          duration,
                             int64_t         pos,
                             int             serial)
{
    VideoState            *is     = ffp->is;
    Frame                 *vp;

    if (!(vp = ffp_frame_queue_peek_writable(&is->pictq)))
        return -1;

    vp->sar.num = 1;
    vp->sar.den = 1;

    /* alloc or resize hardware picture buffer */
    if (!vp->bmp || vp->reallocate || !vp->allocated ||
        vp->width  != picture->width ||
        vp->height != picture->height) {

        if (vp->width != picture->width || vp->height != picture->height)
            ffp_notify_msg3(ffp, FFP_MSG_VIDEO_SIZE_CHANGED, (int)picture->width, (int)picture->height);

        vp->allocated  = 0;
        vp->reallocate = 0;
        vp->width      = (int)picture->width;
        vp->height     = (int)picture->height;

        /* the allocation must be done in the main thread to avoid
         locking problems. */
        vtb_alloc_picture(ffp);

        if (is->videoq.abort_request)
            return -1;
    }
    /* if the frame is not skipped, then display it */
    if (vp->bmp) {
        /* get a pointer on the bitmap */
        SDL_VoutLockYUVOverlay(vp->bmp);
        SDL_VoutOverlayVideoToolBox_FillFrame(vp->bmp,picture);
        /* update the bitmap content */
        SDL_VoutUnlockYUVOverlay(vp->bmp);

        vp->pts = pts;
        vp->duration = duration;
        vp->pos = pos;
        vp->serial = serial;
        ffp_frame_queue_push(&is->pictq);
    }
    return 0;
}



static CFDictionaryRef CreateDictionaryWithPkt(double time, double dts, double pts,double pkt_serial)
{
    CFStringRef key[4] = {
        CFSTR("PKT_SORT"),
        CFSTR("PKT_DTS"),
        CFSTR("PKT_PTS"),
        CFSTR("PKT_SERIAL")
    };
    CFNumberRef value[4];
    CFDictionaryRef display_time;

    value[0] = CFNumberCreate(kCFAllocatorDefault, kCFNumberDoubleType, &time);
    value[1] = CFNumberCreate(kCFAllocatorDefault, kCFNumberDoubleType, &dts);
    value[2] = CFNumberCreate(kCFAllocatorDefault, kCFNumberDoubleType, &pts);
    value[3] = CFNumberCreate(kCFAllocatorDefault, kCFNumberDoubleType, &pkt_serial);

    display_time = CFDictionaryCreate(
                                      kCFAllocatorDefault, (const void **)&key, (const void **)&value, 4,
                                      &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

    CFRelease(value[0]);
    CFRelease(value[1]);
    CFRelease(value[2]);
    CFRelease(value[3]);
    return display_time;
}



static void GetPktTSFromRef(CFDictionaryRef inFrameInfoDictionary, sort_queue *frame)
{
    frame->sort = -1.0;
    frame->dts = AV_NOPTS_VALUE;
    frame->pts = AV_NOPTS_VALUE;
    frame->serial = AV_NOPTS_VALUE;
    if (inFrameInfoDictionary == NULL)
        return;

    CFNumberRef value[4];

    value[0] = (CFNumberRef)CFDictionaryGetValue(inFrameInfoDictionary, CFSTR("PKT_SORT"));
    if (value[0])
        CFNumberGetValue(value[0], kCFNumberDoubleType, &frame->sort);
    value[1] = (CFNumberRef)CFDictionaryGetValue(inFrameInfoDictionary, CFSTR("PKT_DTS"));
    if (value[1])
        CFNumberGetValue(value[1], kCFNumberDoubleType, &frame->dts);
    value[2] = (CFNumberRef)CFDictionaryGetValue(inFrameInfoDictionary, CFSTR("PKT_PTS"));
    if (value[2])
        CFNumberGetValue(value[2], kCFNumberDoubleType, &frame->pts);
    value[3] = (CFNumberRef)CFDictionaryGetValue(inFrameInfoDictionary, CFSTR("PKT_SERIAL"));
    if (value[3])
        CFNumberGetValue(value[3], kCFNumberDoubleType, &frame->serial);
    return;
}


void QueuePicture(VideoToolBoxContext* ctx) {
    VTBPicture picture;
    if (true == GetVTBPicture(ctx, &picture)) {
        double pts;
        double duration;
        int64_t vtb_pts_us = (int64_t)picture.pts;
        int64_t videotoolbox_pts = vtb_pts_us;

        AVRational tb = ctx->ffp->is->video_st->time_base;
        AVRational frame_rate = av_guess_frame_rate(ctx->ffp->is->ic, ctx->ffp->is->video_st, NULL);
        duration = (frame_rate.num && frame_rate.den ? av_q2d((AVRational) {frame_rate.den, frame_rate.num}) : 0);
        pts = (videotoolbox_pts == AV_NOPTS_VALUE) ? NAN : videotoolbox_pts * av_q2d(tb);

        vtb_queue_picture(ctx->ffp, &picture, pts, duration, 0,  ctx->ffp->is->viddec.pkt_serial);
        CVBufferRelease(picture.cvBufferRef);

        SortQueuePop(ctx);
    } else {
        ALOGI("Get Picture failure!!!\n");
    }
}


void VTDecoderCallback(void *decompressionOutputRefCon,
                       void *sourceFrameRefCon,
                       OSStatus status,
                       VTDecodeInfoFlags infoFlags,
                       CVImageBufferRef imageBuffer,
                       CMTime presentationTimeStamp,
                       CMTime presentationDuration)
{
    @autoreleasepool {
        VideoToolBoxContext *ctx = (VideoToolBoxContext*)decompressionOutputRefCon;
        if (!ctx || ctx->dealloced == true) {
            return;
        }
        sort_queue *newFrame = (sort_queue*)malloc(sizeof(sort_queue));
        newFrame->nextframe = NULL;
        GetPktTSFromRef(sourceFrameRefCon, newFrame);
        ctx->last_sort = newFrame->sort;
        CFRelease(sourceFrameRefCon);
        if (status != 0) {
            ALOGI("decode callback  %d \n", (int)status);
            ALOGI("Signal: status\n");
            goto failed;
        }

        if (ctx->refresh_session) {
            goto failed;
        }

        if (newFrame->serial != ctx->serial) {
            goto failed;
        }

        if (imageBuffer == NULL) {
            ALOGI("imageBuffer null\n");
            goto failed;
        }


        OSType format_type = CVPixelBufferGetPixelFormatType(imageBuffer);
        if (format_type != kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange) {
            ALOGI("format_type error \n");
            goto failed;
        }
        if (kVTDecodeInfo_FrameDropped & infoFlags) {
            ALOGI("droped\n");
            goto failed;
        }

        if (ctx->new_seg_flag) {
            ALOGI("new seg process!!!!");
            while (ctx->m_queue_depth > 0) {
                QueuePicture(ctx);
            }
            ctx->new_seg_flag = false;
        }

        if (ctx->m_sort_queue && newFrame->pts < ctx->m_sort_queue->pts) {
            goto failed;
        }

        if (CVPixelBufferIsPlanar(imageBuffer)) {
            newFrame->width  = CVPixelBufferGetWidthOfPlane(imageBuffer, 0);
            newFrame->height = CVPixelBufferGetHeightOfPlane(imageBuffer, 0);
        } else {
            newFrame->width  = CVPixelBufferGetWidth(imageBuffer);
            newFrame->height = CVPixelBufferGetHeight(imageBuffer);
        }


        newFrame->pixel_buffer_ref = CVBufferRetain(imageBuffer);
        if (newFrame->pts != AV_NOPTS_VALUE) {
            newFrame->sort = newFrame->pts;
        } else {
            newFrame->sort = newFrame->dts;
        }
        pthread_mutex_lock(&ctx->m_queue_mutex);
        volatile sort_queue *queueWalker = ctx->m_sort_queue;
        if (!queueWalker || (newFrame->sort < queueWalker->sort)) {
            newFrame->nextframe = queueWalker;
            ctx->m_sort_queue = newFrame;
        } else {
            bool frameInserted = false;
            volatile sort_queue *nextFrame = NULL;
            while (!frameInserted) {
                nextFrame = queueWalker->nextframe;
                if (!nextFrame || (newFrame->sort < nextFrame->sort)) {
                    newFrame->nextframe = nextFrame;
                    queueWalker->nextframe = newFrame;
                    frameInserted = true;
                }
                queueWalker = nextFrame;
            }
        }
        ctx->m_queue_depth++;
        pthread_mutex_unlock(&ctx->m_queue_mutex);

        //ALOGI("%lf %lf %lf \n", newFrame->sort,newFrame->pts, newFrame->dts);
        //ALOGI("display queue deep %d\n", ctx->m_queue_depth);

        if (ctx->ffp->is == NULL || ctx->ffp->is->abort_request || ctx->ffp->is->viddec.queue->abort_request) {
            while (ctx->m_queue_depth > 0) {
                SortQueuePop(ctx);
            }
            goto successed;
        }
        //ALOGI("depth %d  %d\n", ctx->m_queue_depth, ctx->m_max_ref_frames);
        if ((ctx->m_queue_depth > ctx->m_max_ref_frames)) {
            QueuePicture(ctx);
        }
    successed:
        return;
    failed:
        ALOGI("FailedSignal: %lf\n", newFrame->pts);
        if (newFrame) {
            free(newFrame);
        }
        return;
    }
}



void CreateVTBSession(VideoToolBoxContext* context, int width, int height, CMFormatDescriptionRef fmt_desc)
{
    VTDecompressionSessionRef vt_session = NULL;
    CFMutableDictionaryRef destinationPixelBufferAttributes;
    VTDecompressionOutputCallbackRecord outputCallback;
    OSStatus status;
    int width_frame_max = ffpipeline_ios_get_frame_max_width(context->ffp->pipeline);

    if (width > width_frame_max) {
        double w_scaler = (float)width_frame_max / width;
        width = width_frame_max;
        height = height * w_scaler;
    }

    ALOGI("after scale width %d height %d \n", width, height);

    destinationPixelBufferAttributes = CFDictionaryCreateMutable(
                                                                 NULL,
                                                                 0,
                                                                 &kCFTypeDictionaryKeyCallBacks,
                                                                 &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetSInt32(destinationPixelBufferAttributes,
                          kCVPixelBufferPixelFormatTypeKey, kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange);
    CFDictionarySetSInt32(destinationPixelBufferAttributes,
                          kCVPixelBufferWidthKey, width);
    CFDictionarySetSInt32(destinationPixelBufferAttributes,
                          kCVPixelBufferHeightKey, height);
    outputCallback.decompressionOutputCallback = VTDecoderCallback;
    outputCallback.decompressionOutputRefCon = context  ;
    status = VTDecompressionSessionCreate(
                                          kCFAllocatorDefault,
                                          fmt_desc,
                                          NULL,
                                          destinationPixelBufferAttributes,
                                          &outputCallback,
                                          &vt_session);


    if (status != noErr) {
        context->m_vt_session = NULL;
        NSError* error = [NSError errorWithDomain:NSOSStatusErrorDomain code:status userInfo:nil];
        NSLog(@"Error %@", [error description]);
        ALOGI("%s - failed with status = (%d)", __FUNCTION__, (int)status);
    } else {
        context->m_vt_session =(void*) vt_session;
    }
    CFRelease(destinationPixelBufferAttributes);
}



int videotoolbox_decode_video_internal(VideoToolBoxContext* context, AVCodecContext *avctx, const AVPacket *avpkt, int* got_picture_ptr)
{
    OSStatus status                 = 0;
    double sort_time                = GetSystemTime();
    uint32_t decoderFlags           = 0;
    CFDictionaryRef frame_info      = NULL;;
    CMSampleBufferRef sample_buff   = NULL;
    AVIOContext *pb                 = NULL;
    int demux_size                  = 0;
    uint8_t *demux_buff             = NULL;
    uint8_t *pData                  = avpkt->data;
    int iSize                       = avpkt->size;
    double pts                      = avpkt->pts;
    double dts                      = avpkt->dts;

    if (!context) {
        goto failed;
    }

    if (context->refresh_session) {
        decoderFlags |= kVTDecodeFrame_DoNotOutputFrame;
        ALOGI("flag :%d flag %d \n", decoderFlags,avpkt->flags);
    }

    if (context->refresh_request) {

        while (context->m_queue_depth > 0) {
            SortQueuePop(context);
        }

        if(context->m_vt_session) {
            VTDecompressionSessionInvalidate(context->m_vt_session);
            CFRelease(context->m_vt_session);
        }

        CreateVTBSession(context, context->ffp->is->viddec.avctx->width, context->ffp->is->viddec.avctx->height, context->m_fmt_desc);
        context->refresh_request = false;
    }

    if (pts == AV_NOPTS_VALUE) {
        pts = dts;
    }

    frame_info = CreateDictionaryWithPkt(sort_time - context->m_sort_time_offset, dts, pts,context->serial);

    if (context->m_convert_bytestream) {
        ALOGI("the buffer should m_convert_byte\n");
        if(avio_open_dyn_buf(&pb) < 0) {
            goto failed;
        }
        ff_avc_parse_nal_units(pb, pData, iSize);
        demux_size = avio_close_dyn_buf(pb, &demux_buff);
        ALOGI("demux_size:%d\n", demux_size);
        if (demux_size == 0) {
            goto failed;
        }
        sample_buff = CreateSampleBufferFrom(context->m_fmt_desc, demux_buff, demux_size);
    } else if (context->m_convert_3byteTo4byteNALSize) {
        ALOGI("3byteto4byte\n");
        if (avio_open_dyn_buf(&pb) < 0) {
            goto failed;
        }

        uint32_t nal_size;
        uint8_t *end = avpkt->data + avpkt->size;
        uint8_t *nal_start = pData;
        while (nal_start < end) {
            nal_size = AV_RB24(nal_start);
            avio_wb32(pb, nal_size);
            nal_start += 3;
            avio_write(pb, nal_start, nal_size);
            nal_start += nal_size;
        }
        demux_size = avio_close_dyn_buf(pb, &demux_buff);
        sample_buff = CreateSampleBufferFrom(context->m_fmt_desc, demux_buff, demux_size);
    } else {
        sample_buff = CreateSampleBufferFrom(context->m_fmt_desc, pData, iSize);
    }
    if (!sample_buff) {
        if (demux_size) {
            av_free(demux_buff);
        }
        ALOGI("%s - CreateSampleBufferFrom failed", __FUNCTION__);
        goto failed;
    }

    if (avpkt->flags & AV_PKT_FLAG_NEW_SEG) {
        context->new_seg_flag = true;
    }


    context->last_keyframe_pts = avpkt->pts;

    //ALOGI("Decode before \n!!!!!!!");
    status = VTDecompressionSessionDecodeFrame(context->m_vt_session, sample_buff, decoderFlags, (void*)frame_info, 0);
    //ALOGI("Decode after \n!!!!!!!");

    if (status != 0) {
        ALOGE("status %d \n", (int)status);
        if (status == kVTInvalidSessionErr) {
            context->refresh_session = true;
        }
        if (status == kVTVideoDecoderMalfunctionErr) {
            context->recovery_drop_packet = true;
        }
        if (status != 0) {
            goto failed;
        }
    }


    status = VTDecompressionSessionWaitForAsynchronousFrames(context->m_vt_session);

    //ALOGI("Wait : %lf \n",pts);
    if ((sort_time - context->m_sort_time_offset) > context->last_sort) {

    }
    if (status != 0) {
        goto failed;
    }

    if (sample_buff) {
        CFRelease(sample_buff);
    }
    if (demux_size) {
        av_free(demux_buff);
    }

    *got_picture_ptr = 1;
    return 0;
failed:
    if (sample_buff) {
        CFRelease(sample_buff);
    }
    if (demux_size) {
        av_free(demux_buff);
    }
    *got_picture_ptr = 0;
    return -1;
}

static inline void ResetPktBuffer(VideoToolBoxContext* context) {
    for (int i = 0 ; i < context->m_buffer_deep; i++) {
        av_free_packet(&context->m_buffer_packet[i]);
    }
    context->m_buffer_deep = 0;
    memset(context->m_buffer_packet, 0, sizeof(context->m_buffer_packet));
}

static inline void DuplicatePkt(VideoToolBoxContext* context, const AVPacket* pkt) {
    if (context->m_buffer_deep >= MAX_PKT_QUEUE_DEEP) {
        context->idr_based_identified = false;
        ResetPktBuffer(context);
    }
    AVPacket* avpkt = &context->m_buffer_packet[context->m_buffer_deep];
    av_new_packet(avpkt, pkt->size);
    avpkt->flags        = pkt->flags;
    avpkt->pts          = pkt->pts;
    avpkt->dts          = pkt->dts;
    avpkt->duration     = pkt->duration;
    avpkt->pos          = pkt->pos;
    avpkt->size         = pkt->size;
    avpkt->stream_index = pkt->stream_index;
    avpkt->convergence_duration = pkt->convergence_duration;
    memcpy(avpkt->data, pkt->data, pkt->size);
    context->m_buffer_deep++;
}



int videotoolbox_decode_video(VideoToolBoxContext* context, AVCodecContext *avctx, const AVPacket *avpkt, int* got_picture_ptr)
{
    if (!avpkt || !avpkt->data) {
        return 0;
    }

    if (ff_avpacket_is_idr(avpkt) == true) {
        context->idr_based_identified = true;
    }
    if (ff_avpacket_i_or_idr(avpkt, context->idr_based_identified) == true) {
        ResetPktBuffer(context);
        context->recovery_drop_packet = false;
    }
    if (context->recovery_drop_packet == true) {
        return -1;
    }

    DuplicatePkt(context, avpkt);

    if (context->refresh_session) {
        int ret = 0;
        if (context->m_vt_session) {
            VTDecompressionSessionInvalidate(context->m_vt_session);
            CFRelease(context->m_vt_session);
        }

        CreateVTBSession(context, context->ffp->is->viddec.avctx->width, context->ffp->is->viddec.avctx->height, context->m_fmt_desc);

        if ((context->m_buffer_deep > 0) &&
            ff_avpacket_i_or_idr(&context->m_buffer_packet[0], context->idr_based_identified) == true ) {
            for (int i = 0; i < context->m_buffer_deep; i++) {
                AVPacket* pkt = &context->m_buffer_packet[i];
                ret = videotoolbox_decode_video_internal(context, avctx, pkt, got_picture_ptr);
            }
        } else {
            context->recovery_drop_packet = true;
            ret = -1;
            ALOGE("recovery error!!!!\n");
        }
        context->refresh_session = false;
        return ret;
    }
    return videotoolbox_decode_video_internal(context, avctx, avpkt, got_picture_ptr);
}


static void dict_set_string(CFMutableDictionaryRef dict, CFStringRef key, const char * value)
{
    CFStringRef string;
    string = CFStringCreateWithCString(NULL, value, kCFStringEncodingASCII);
    CFDictionarySetValue(dict, key, string);
    CFRelease(string);
}

static void dict_set_boolean(CFMutableDictionaryRef dict, CFStringRef key, BOOL value)
{
    CFDictionarySetValue(dict, key, value ? kCFBooleanTrue: kCFBooleanFalse);
}


static void dict_set_object(CFMutableDictionaryRef dict, CFStringRef key, CFTypeRef *value)
{
    CFDictionarySetValue(dict, key, value);
    CFRelease(value);
}

static void dict_set_data(CFMutableDictionaryRef dict, CFStringRef key, uint8_t * value, uint64_t length)
{
    CFDataRef data;
    data = CFDataCreate(NULL, value, length);
    CFDictionarySetValue(dict, key, data);
    CFRelease(data);
}

static void dict_set_i32(CFMutableDictionaryRef dict, CFStringRef key,
                         int32_t value)
{
    CFNumberRef number;
    number = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
    CFDictionarySetValue(dict, key, number);
    CFRelease(number);
}

static CMFormatDescriptionRef CreateFormatDescriptionFromCodecData(Uint32 format_id, int width, int height, const uint8_t *extradata, int extradata_size, uint32_t atom)
{
    CMFormatDescriptionRef fmt_desc = NULL;
    OSStatus status;

    CFMutableDictionaryRef par = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks,&kCFTypeDictionaryValueCallBacks);
    CFMutableDictionaryRef atoms = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks,&kCFTypeDictionaryValueCallBacks);
    CFMutableDictionaryRef extensions = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

    /* CVPixelAspectRatio dict */
    dict_set_i32(par, CFSTR ("HorizontalSpacing"), 0);
    dict_set_i32(par, CFSTR ("VerticalSpacing"), 0);
    /* SampleDescriptionExtensionAtoms dict */
    dict_set_data(atoms, CFSTR ("avcC"), (uint8_t *)extradata, extradata_size);

      /* Extensions dict */
    dict_set_string(extensions, CFSTR ("CVImageBufferChromaLocationBottomField"), "left");
    dict_set_string(extensions, CFSTR ("CVImageBufferChromaLocationTopField"), "left");
    dict_set_boolean(extensions, CFSTR("FullRangeVideo"), FALSE);
    dict_set_object(extensions, CFSTR ("CVPixelAspectRatio"), (CFTypeRef *) par);
    dict_set_object(extensions, CFSTR ("SampleDescriptionExtensionAtoms"), (CFTypeRef *) atoms);
    status = CMVideoFormatDescriptionCreate(NULL, format_id, width, height, extensions, &fmt_desc);

    if (status == 0)
        return fmt_desc;
    else
        return NULL;
}

void dealloc_videotoolbox(VideoToolBoxContext* context)
{
    while (context && context->m_queue_depth > 0) {
        SortQueuePop(context);
    }
    if (context && context->m_vt_session) {
        VTDecompressionSessionInvalidate(context->m_vt_session);
        CFRelease(context->m_vt_session);
    }
    if (context) {
        ResetPktBuffer(context);
        context->dealloced = true;
    }
}



VideoToolBoxContext* init_videotoolbox(FFPlayer* ffp, AVCodecContext* ic)
{
    int width           = ic->width;
    int height          = ic->height;
    int level           = ic->level;
    int profile         = ic->profile;
    int sps_level       = 0;
    int sps_profile     = 0;
    int extrasize       = ic->extradata_size;
    int codec           = ic->codec_id;
    uint8_t* extradata  = ic->extradata;

    VideoToolBoxContext* context_vtb = malloc(sizeof(VideoToolBoxContext));
    if (!context_vtb) {
        goto failed;
    }

    context_vtb->m_convert_bytestream = false;
    context_vtb->m_convert_3byteTo4byteNALSize = false;
    context_vtb->refresh_request = false;
    context_vtb->new_seg_flag = false;
    context_vtb->recovery_drop_packet = false;
    context_vtb->refresh_session = false;
    context_vtb->idr_based_identified = true;
    context_vtb->dealloced = false;
    context_vtb->last_keyframe_pts = 0;
    context_vtb->m_max_ref_frames = 0;
    context_vtb->ffp = ffp;
    context_vtb->serial = 0;
    context_vtb->last_sort = 0;
    context_vtb->m_buffer_deep = 0;
    memset(context_vtb->m_buffer_packet, 0, sizeof(context_vtb->m_buffer_packet));

    switch (profile) {
        case FF_PROFILE_H264_HIGH_10:
        case FF_PROFILE_H264_HIGH_10_INTRA:
        case FF_PROFILE_H264_HIGH_422:
        case FF_PROFILE_H264_HIGH_422_INTRA:
        case FF_PROFILE_H264_HIGH_444_PREDICTIVE:
        case FF_PROFILE_H264_HIGH_444_INTRA:
        case FF_PROFILE_H264_CAVLC_444:
            goto failed;
    }

    if (width < 0 || height < 0) {
        goto failed;
    }

    switch (codec) {
        case AV_CODEC_ID_H264:
            if (extrasize < 7 || extradata == NULL) {
                ALOGI("%s - avcC atom too data small or missing", __FUNCTION__);
                goto failed;
            }

            if (extradata[0] == 1) {
                if (!validate_avcC_spc(extradata, extrasize, &context_vtb->m_max_ref_frames, &sps_level, &sps_profile)) {
                    //goto failed;
                }
                if (level == 0 && sps_level > 0)
                    level = sps_level;

                if (profile == 0 && sps_profile > 0)
                    profile = sps_profile;
                if (profile == FF_PROFILE_H264_MAIN && level == 32 && context_vtb->m_max_ref_frames > 4) {
                    ALOGE("%s - Main@L3.2 detected, VTB cannot decode with %d ref frames", __FUNCTION__, context_vtb->m_max_ref_frames);
                    goto failed;
                }

                if (extradata[4] == 0xFE) {
                    extradata[4] = 0xFF;
                    context_vtb->m_convert_3byteTo4byteNALSize = true;
                }

                context_vtb->m_fmt_desc = CreateFormatDescriptionFromCodecData(IJK_VTB_FCC_AVC1, width, height, extradata, extrasize,  IJK_VTB_FCC_AVC);

                ALOGI("%s - using avcC atom of size(%d), ref_frames(%d)", __FUNCTION__, extrasize, context_vtb->m_max_ref_frames);
            } else {
                    if ((extradata[0] == 0 && extradata[1] == 0 && extradata[2] == 0 && extradata[3] == 1) ||
                    (extradata[0] == 0 && extradata[1] == 0 && extradata[2] == 1)) {
                        AVIOContext *pb;
                        if (avio_open_dyn_buf(&pb) < 0) {
                            goto failed;
                        }

                        context_vtb->m_convert_bytestream = true;
                        ff_isom_write_avcc(pb, extradata, extrasize);
                        extradata = NULL;

                        extrasize = avio_close_dyn_buf(pb, &extradata);

                        if (!validate_avcC_spc(extradata, extrasize, &context_vtb->m_max_ref_frames, &sps_level, &sps_profile)) {
                            av_free(extradata);
                            goto failed;
                        }

                        context_vtb->m_fmt_desc = CreateFormatDescriptionFromCodecData(IJK_VTB_FCC_AVC1, width, height, extradata, extrasize, IJK_VTB_FCC_AVC);

                        av_free(extradata);
                    } else {
                        ALOGI("%s - invalid avcC atom data", __FUNCTION__);
                        goto failed;
                    }
                }
            context_vtb->m_pformat_name = "vtb-h264";
            break;
        default:
            goto failed;
    }
    if (context_vtb->m_fmt_desc == NULL) {
        context_vtb->m_pformat_name = "";
        goto failed;
    }


    CreateVTBSession(context_vtb, width, height, context_vtb->m_fmt_desc);
    context_vtb->m_sort_queue = 0;
    if (context_vtb->m_vt_session == NULL) {
        if (context_vtb->m_fmt_desc) {
            CFRelease(context_vtb->m_fmt_desc);
            context_vtb->m_fmt_desc = NULL;
        }
        context_vtb->m_pformat_name = "";
        goto failed;
    }
    context_vtb->width = width;
    context_vtb->height = height;
    context_vtb->m_queue_depth = 0;
    memset(&context_vtb->m_videobuffer, 0, sizeof(VTBPicture));
    if (context_vtb->m_max_ref_frames <= 0) {
        context_vtb->m_max_ref_frames = 2;
    }
    if (context_vtb->m_max_ref_frames > 5) {
        context_vtb->m_max_ref_frames = 5;
    }

    ALOGI("m_max_ref_frames %d \n", context_vtb->m_max_ref_frames);

    context_vtb->m_sort_time_offset = GetSystemTime();

    return context_vtb;

failed:
    if (context_vtb) {
        free(context_vtb);
    }
    return NULL;
}
