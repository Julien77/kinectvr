#pragma once

#ifndef Sound_h__
#define Sound_h__

#include "cinder/app/AppBasic.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/Callback.h"
#include "cinder/Rand.h"
#include "cinder/CinderMath.h"
#include "cinder/Cinder.h"
#include "cinder/audio/PcmBuffer.h"

using namespace ci;
using namespace ci::app;

class Sound 
{
    public:
    Sound();
    ~Sound();
	void update(float zPos);    
};

#endif // Sound_h__