#pragma once

#if  defined(_MSC_VER) || defined(_WIN32)
//Windows stuff
#define RES_SOUND1			128

#else
#include "cinder/CinderResources.h"

#define RES_SOUND1			CINDER_RESOURCE( ../resources/, guitar.mp3, 128, MP3 )
//#define RES_MY_RES			CINDER_RESOURCE( ../resources/, image_name.png, 129, IMAGE )
#endif
