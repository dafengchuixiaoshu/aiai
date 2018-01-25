/*
 * ijkplayer_ios.h
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

#include "ijkplayer/ijkplayer.h"
#import "IJKSDLGLView.h"

// ref_count is 1 after open
IjkMediaPlayer *ijkmp_ios_create(int (*msg_loop)(void*));
void ijkmp_ios_destroy(IjkMediaPlayer** mp);

void            ijkmp_ios_set_filterdata(IjkMediaPlayer *mp, unsigned int filter_type,bool is_open_volume,const char *first_movie_path,const char *second_movie_path,float start_time,float end_time,int fps);
void            ijkmp_ios_set_glview(IjkMediaPlayer *mp, IJKSDLGLView *glView);
void            ijkmp_ios_set_videotoolbox_enabled(IjkMediaPlayer *mp, BOOL enabled);
void            ijkmp_ios_set_frame_max_width(IjkMediaPlayer *mp, int width);
bool            ijkmp_ios_is_videotoolbox_open(IjkMediaPlayer *mp);
