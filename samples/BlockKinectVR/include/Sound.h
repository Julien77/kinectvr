#pragma once

#include "cinder/audio/PcmBuffer.h"
#include "cinder/audio/Output.h"

class Sound
{
    public:
        Sound();
        ~Sound();
        void setup();
        void update(float mPosZ);
    
    private:
        cinder::audio::TrackRef mTrack1;
        cinder::audio::PcmBuffer32fRef mPcmBuffer1;
    
};
