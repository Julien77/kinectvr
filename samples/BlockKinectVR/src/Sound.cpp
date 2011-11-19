#include "Sound.h"
#include "Resources.h"

#include "cinder/app/AppBasic.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/PcmBuffer.h"

using namespace ci;
using namespace ci::app;

Sound::Sound()
{
}

Sound::~Sound()
{
}

void Sound::setup()
{
    try 
    {
        //add the audio track the default audio output
        mTrack1 = audio::Output::addTrack( audio::load( loadResource( RES_SOUND1 ) ) );
        
        //you must enable enable PCM buffering on the track to be able to call getPcmBuffer on it later
        //mTrack1->enablePcmBuffering( true );
        
        //debug info
        console() << "(App) Able to load and play the sound1." << std::endl;
    } 
    catch ( ... ) {
        console() << "(App) Unable to load or play the sound1!" << std::endl;
    }
    mTrack1->setLooping( true );
}

void Sound::update(float mPosZ)
{
    //get the latest pcm buffer from the track
	//mPcmBuffer1 = mTrack1->getPcmBuffer();
    
    //mPoz must be transform to mVolume
    float mVolume = -0.01*mPosZ+1;
    mTrack1->setVolume(mVolume);//Must be between [0,1]
}







