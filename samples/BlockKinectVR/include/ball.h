#pragma once

#ifndef Ball_h__
#define Ball_h__


#include "cinder/gl/gl.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include "cinder/imageio.h"


using namespace ci;


class BlockOpenNISampleAppApp;
class MenFigure;
class Room;


class Ball
{
public:
	Ball();
	~Ball();
	void setup(Vec3f pos, Vec3f vel,MenFigure* fig, Room* room);
	void update(Vec3f pos, bool put);
	void draw();
protected:
private:
	float mRadius;
	Vec3f mPos;
	Vec3f mPrevPos;
	Vec3f mVel;
	float mMass;
	MenFigure* mFig;
	Room* mRoom;

	Url					mBallUrl;
	gl::Texture         mBallTex;
};

















#endif // Ball_h__