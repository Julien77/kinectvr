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
	void setup();
	void update(Vec3f pos);
	void draw();
protected:
private:
	Vec3f mPos;
};

















#endif // figure_h__