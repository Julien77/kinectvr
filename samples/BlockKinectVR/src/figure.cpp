#include "figure.h"


Figure::Figure():mPos(0,0,0)
{

}

Figure::~Figure()
{

}


void Figure::setup()
{
	this->mPos = Vec3f(0,0,0);
}

void Figure::update( Vec3f pos )
{
	this->mPos = pos;
}

void Figure::draw()
{
	glPushMatrix();
		glColor4f( ColorA( 1.0f, 0.0f, 0.0f, 1.0f ) );
		gl::translate(this->mPos);
		gl::drawCylinder(5, 5, 10, 12);
		//gl::drawSphere(Vec3f(0,0,0),10,12);
	glPopMatrix();
}

