/*
 * ijkplayer_ios.c
 *
 * Copyright (c) 2013 Zhang Rui <bbcallen@gmail.com>
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

#import "ijkplayer_ios.h"

#import "ijksdl/ios/ijksdl_ios.h"

#include <stdio.h>
#include <assert.h>
#include "ijkplayer/ff_fferror.h"
#include "ijkplayer/ff_ffplay.h"
#include "ijkplayer/ijkplayer_internal.h"
#include "ijkplayer/pipeline/ffpipeline_ffplay.h"
#include "ffpipeline_ios.h"

IjkMediaPlayer *ijkmp_ios_create(int (*msg_loop)(void*))
{
    IjkMediaPlayer *mp = ijkmp_create(msg_loop);
    if (!mp)
        goto fail;

    mp->ffplayer->vout = SDL_VoutIos_CreateForGLES2();
    if (!mp->ffplayer->vout)
        goto fail;

    mp->ffplayer->aout = SDL_AoutIos_CreateForAudioUnit();
    if (!mp->ffplayer->vout)
        goto fail;

    mp->ffplayer->pipeline = ffpipeline_create_from_ios(mp->ffplayer);
    if (!mp->ffplayer->pipeline)
        goto fail;

    return mp;

fail:
    ijkmp_dec_ref_p(&mp);
    return NULL;
}

void ijkmp_ios_destroy(IjkMediaPlayer** mp){
    ijkmp_dec_ref_p(mp);
}

void ijkmp_ios_set_filterdata(IjkMediaPlayer *mp, unsigned int filter_type,bool is_open_volume,const char *first_movie_path,const char *second_movie_path,float start_time,float end_time,int fps){
    
    FilterDataStructure* data = (FilterDataStructure*)malloc(sizeof(FilterDataStructure));
    memset(data, 0, sizeof(FilterDataStructure));
    filterDataStructure_init(data);
    
    data->filter_type = filter_type;
    data->is_open_volume = is_open_volume;
    if (first_movie_path) {
        data->first_movie_path = (char*)malloc(strlen(first_movie_path)+1);
        memset(data->first_movie_path, 0, strlen(first_movie_path)+1);
        strcpy(data->first_movie_path, first_movie_path);
    }
    if (second_movie_path) {
        data->second_movie_path = (char*)malloc(strlen(second_movie_path)+1);
        memset(data->second_movie_path, 0, strlen(second_movie_path)+1);
        strcpy(data->second_movie_path, second_movie_path);
    }
    data->start_time = start_time;
    data->end_time = end_time;
    data->fps = fps;
    
    ijkmp_create_required_filter(mp, data);
}

void ijkmp_ios_set_glview_l(IjkMediaPlayer *mp, IJKSDLGLView *glView)
{
    assert(mp);
    assert(mp->ffplayer);
    assert(mp->ffplayer->vout);

    SDL_VoutIos_SetGLView(mp->ffplayer->vout, glView);
}

void ijkmp_ios_set_glview(IjkMediaPlayer *mp, IJKSDLGLView *glView)
{
    assert(mp);
    MPTRACE("ijkmp_ios_set_view(glView=%p)\n", (void*)glView);
    pthread_mutex_lock(&mp->mutex);
    ijkmp_ios_set_glview_l(mp, glView);
    pthread_mutex_unlock(&mp->mutex);
    MPTRACE("ijkmp_ios_set_view(glView=%p)=void\n", (void*)glView);
}

void ijkmp_ios_set_frame_max_width_l(IjkMediaPlayer *mp, int width)
{
    assert(mp);
    assert(mp->ffplayer);
    assert(mp->ffplayer->pipeline);
    ffpipeline_ios_set_frame_max_width(mp->ffplayer->pipeline, width);
}

void ijkmp_ios_set_frame_max_width(IjkMediaPlayer *mp, int width)
{
    assert(mp);
    MPTRACE("%s (width=%d)\n", __func__, width);
    pthread_mutex_lock(&mp->mutex);
    ijkmp_ios_set_frame_max_width_l(mp, width);
    pthread_mutex_unlock(&mp->mutex);
    MPTRACE("%s after(width=%d)\n", __func__, width);
}

void ijkmp_ios_set_videotoolbox_enabled_l(IjkMediaPlayer *mp, BOOL enabled)
{
    assert(mp);
    assert(mp->ffplayer);
    assert(mp->ffplayer->pipeline);
    if (enabled == YES) {
        ffpipeline_ios_set_videotoolbox_enabled(mp->ffplayer->pipeline, 1);
    } else {
        ffpipeline_ios_set_videotoolbox_enabled(mp->ffplayer->pipeline, 0);
    }
}

void ijkmp_ios_set_videotoolbox_enabled(IjkMediaPlayer *mp, BOOL enabled)
{
    assert(mp);
    MPTRACE("%s enable(EnableFlag=%d)\n", __func__, enabled);
    pthread_mutex_lock(&mp->mutex);
    ijkmp_ios_set_videotoolbox_enabled_l(mp, enabled);
    pthread_mutex_unlock(&mp->mutex);
    MPTRACE("%s enable(EnableFlag=%d)\n", __func__, enabled);
}

bool ijkmp_ios_is_videotoolbox_open_l(IjkMediaPlayer *mp)
{
    assert(mp);
    assert(mp->ffplayer);

    return false;
}

bool ijkmp_ios_is_videotoolbox_open(IjkMediaPlayer *mp)
{
    assert(mp);
    MPTRACE("%s()\n", __func__);
    pthread_mutex_lock(&mp->mutex);
    bool ret = ijkmp_ios_is_videotoolbox_open_l(mp);
    pthread_mutex_unlock(&mp->mutex);
    MPTRACE("%s()=%d\n", __func__, ret ? 1 : 0);
    return ret;
}
