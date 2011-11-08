#pragma once

#ifndef room_h__
#define room_h__

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"
#include "cinder/imageio.h"


using namespace ci;
using namespace std;



class Room
{
public:
	Room();
	~Room();	
	void setup(Vec3f cornerOne, Vec3f cornerTwo);
	void update(Vec3f eye ,Vec3f center,Vec3f up);
	void draw();
    void convertCoord(Vec3f *vec);
    
    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;

private:

	// 			  ____ corner two
	//  		 /___/|
	// 			| |_|_|
	// 			|/__|/
	//  corner one

	// We will only use rooms which walls are parallel to axes, so two 
	// corners define the room. (see figure and constructor)

	Vec3f mbottom1;
	Vec3f mbottom2;
	Vec3f mbottom3;
	Vec3f mbottom4;
	Vec3f mtop1;
	Vec3f mtop2;
	Vec3f mtop3;
	Vec3f mtop4;
    
	// Textures and resources used. So far determined with urls which should be later changed to local resources
	Url					mFloorUrl;
	gl::Texture         mFloorTex;
	Url					mBallUrl;
	gl::Texture         mBallTex;
	Url					mWallUrl;
	gl::Texture         mWallTex;


	// PARAMS: this is the on screen parameter display (probably only needed for testing)
	params::InterfaceGl	mParams;

	// CAMERA: camera parameters
	CameraPersp			mCam;
	Quatf				mSceneRotation;
	float				mCamZ;						//< these coordinates make up mEye
	float				mCamX;
	float				mCamY;
	Vec3f				mEye, mCenter, mUp;			//< to determine the viewpoint
	

};







#endif // room_h__