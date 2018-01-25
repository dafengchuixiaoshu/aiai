/*****************************************************************************
 * IJKVideoToolBox.h
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

#ifndef IJKMediaPlayer_videotoolbox_core_h
#define IJKMediaPlayer_videotoolbox_core_h


#import <VideoToolbox/VideoToolbox.h>
#include "ff_ffinc.h"
#include "ff_fferror.h"
#include "ff_ffmsg.h"
#include "ff_ffplay.h"


#define MAX_PKT_QUEUE_DEEP 350

typedef struct VTBPicture {
    double              pts;
    double              dts;
    double              sort;
    CVPixelBufferRef    cvBufferRef;
    uint64_t            width;
    uint64_t            height;
} VTBPicture;


typedef struct sort_queue {
    double              dts;
    double              pts;
    double              serial;
    double              sort;
    int64_t             width;
    int64_t             height;
    CVPixelBufferRef    pixel_buffer_ref;
    volatile struct sort_queue  *nextframe;
} sort_queue;


typedef struct VideoToolBoxContext {
    FFPlayer                   *ffp;
    int                         width;
    int                         height;
    volatile bool               refresh_request;
    volatile bool               new_seg_flag;
    volatile bool               idr_based_identified;
    int64_t                     last_keyframe_pts;
    volatile bool               refresh_session;
    volatile bool               recovery_drop_packet;
    VTDecompressionSessionRef   m_vt_session;
    CMFormatDescriptionRef      m_fmt_desc;
    const char                 *m_pformat_name;
    VTBPicture                  m_videobuffer;
    double                      m_sort_time_offset;
    pthread_mutex_t             m_queue_mutex;
    volatile sort_queue        *m_sort_queue;
    volatile int32_t            m_queue_depth;
    int32_t                     m_max_ref_frames;
    bool                        m_convert_bytestream;
    bool                        m_convert_3byteTo4byteNALSize;
    double                      serial;
    volatile double             last_sort;
    bool                        dealloced;
    int                         m_buffer_deep;
    AVPacket                    m_buffer_packet[MAX_PKT_QUEUE_DEEP];
} VideoToolBoxContext ;


VideoToolBoxContext* init_videotoolbox(FFPlayer* ffp, AVCodecContext* ic);

int videotoolbox_decode_video(VideoToolBoxContext* context, AVCodecContext *avctx, const AVPacket *avpkt,int* got_picture_ptr);

void dealloc_videotoolbox(VideoToolBoxContext* context);

#endif
