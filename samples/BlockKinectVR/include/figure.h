#pragma once

#ifndef figure_h__
#define figure_h__


#include "cinder/gl/gl.h"
#include "cinder/Vector.h"

using namespace ci;


class Figure
{
public:
	Figure();
	~Figure();
	void setup(float radius);
	void update(Vec3f pos);
	void Figure::update( Vec3f cp1,Vec3f cp2 );
	void draw();

	bool isCollisionWithBall(Vec3f center,float radius);
	void setBallVelocityAfterCollsision(Vec3f* vel,Vec3f center,float radius);

protected:
private:
	Vec3f mPos;
	float mRadius;
	Vec3f mHalfAxe;
	Vec3f mPrevPos;
	Vec3f mVel;
	float mMass;

};



class MenFigure
{
public:
	MenFigure();
	~MenFigure();
	void setup(std::vector<float>radiusParams);
	void update(float* x, float * y, float *z, int n);		//< first coordinate should be the head NOT yet implemented, so now it takes pair numbers only
	void draw();

	bool isCollisionWithBall(Vec3f center,float radius);
	void setBallVelocityAfterCollsision(Vec3f* vel,Vec3f center,float radius);

protected:
private:
	int partNum;
	std::vector<Figure> bodyParts;
	unsigned int collisionID;

};

















#endif // figure_h__