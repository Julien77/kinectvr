#include "ball.h"
#include "figure.h"
#include "room.h"
#include "Resources.h"

#include "cinder/app/AppBasic.h"



Ball::Ball():mPos(0,0,0),mMass(2),mRadius(10)
{
	
}

Ball::~Ball()
{

}


void Ball::setup(Vec3f pos, Vec3f vel, MenFigure* fig,Room * room)
{
    //this->mBallUrl= Url( "http://superstreaker.zescript.info/texture_ball.jpg");
	//this->mBallTex  = gl::Texture( loadImage( loadUrl( this->mBallUrl ) ) );
    this->mBallTex  = gl::Texture( loadImage( ci::app::loadResource( RES_IMAGE3 ) )  );
    
	this->mPos = pos;
	this->mPrevPos = pos;
	this->mVel = vel;
	this->mFig=fig;
	this->mRoom = room;

	
}

void Ball::update( Vec3f pos, bool put )
{
	if (put)
	{
		this->mPos = pos;
	}
	else
	{
		static float h = 0.1f;  //times 
		//update with vel and graph
		this->mVel *= 0.99f;
		// gravity
		Vec3f gravity = Vec3f(0,-9.0f,0);
		this->mVel += gravity * h;
		
		Vec3f tmp = this->mPrevPos;
		this->mPrevPos = this->mPos;
		this->mPos += mVel*h;

/*
		float dissipation=0.8f;


		if (this->mPos.y<10)
		{
			this->mPos.y=10;
			this->mVel.y *=-dissipation;
		}
		if (this->mPos.y>90)
		{
			this->mPos.y=90;
			this->mVel.y *=-dissipation;
		}
		if (this->mPos.x<10)
		{
			this->mPos.x=10;
			this->mVel.x *=-dissipation;
		}
		if (this->mPos.x>90)
		{
			this->mPos.x=90;
			this->mVel.x *=-dissipation;
		}
		if (this->mPos.z<-90)
		{
			this->mPos.z=-90;
			this->mVel.z *=-dissipation;
		}
		if (this->mPos.z>-10)
		{
			this->mPos.z=-10;
			this->mVel.z *=-dissipation;
		}*/

		//collsision detection with room
		if(this->mRoom->isCollisionWithBall(this->mPos,this->mRadius))
		{
			this->mRoom->setBallVelocityAfterCollision(&(this->mVel),&(this->mPos),this->mRadius);
			// set back the state
			//this->mPos = this->mPrevPos;
			//this->mPrevPos = tmp;
			// calculate again
			//this->mPrevPos = this->mPos;
			//this->mPos += mVel*h;
		}

		//detect collision with figure
		if(this->mFig->isCollisionWithBall(this->mPos,this->mRadius))
		{
			this->mFig->setBallVelocityAfterCollsision(&(this->mVel),this->mPos,this->mRadius);
			// set back the state
			this->mPos = this->mPrevPos;
			this->mPrevPos = tmp;
			// calculate again
			this->mPrevPos = this->mPos;
			this->mPos += mVel*h;
		}

	}
}

void Ball::draw()
{
	glPushMatrix();
		gl::translate(this->mPos);
		glEnable( GL_TEXTURE_2D);
			this->mBallTex.bind();
			gl::drawSphere(Vec3f(0,0,0),this->mRadius,12);
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

