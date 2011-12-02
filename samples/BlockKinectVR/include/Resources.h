#pragma once

#if  defined(_MSC_VER) || defined(_WIN32)
//Windows stuff
#define RES_SOUND1			128
#define RES_IMAGE1          129
#define RES_IMAGE2          130
#define RES_IMAGE3          131
#define RES_IMAGE4          132

#else
#include "cinder/CinderResources.h"

#define RES_SOUND1			CINDER_RESOURCE( ../resources/, guitar.mp3, 128, MP3 )
#define RES_IMAGE1			CINDER_RESOURCE( ../resources/, Stone_Dungeon_Floor_by_pfunked.jpg, 129, IMAGE )
#define RES_IMAGE2          CINDER_RESOURCE( ../resources/, brick_wall_with_painted_sign.jpg, 130, IMAGE )
#define RES_IMAGE3          CINDER_RESOURCE( ../resources/, texture_ball.jpg, 131, IMAGE )
#define RES_IMAGE4          CINDER_RESOURCE( ../resources/, fofo.jpg, 132, IMAGE )
#endif
