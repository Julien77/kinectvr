#include "figure.h"

#include "cinder/app/AppBasic.h"


Figure::Figure():mPos(0,0,0)
{

}

Figure::~Figure()
{

}


void Figure::setup(float radius)
{
	this->mPos = Vec3f(0,0,0);
	this->mRadius = radius;
	this->mHalfAxe=Vec3f(0,5,0);
}

void Figure::update( Vec3f pos )
{
	float h = 0.1;
	this->mPrevPos = this->mPos;
	this->mPos = pos;
	this->mVel = ( this->mPos - this->mPrevPos ) / h ;
}

void Figure::update( Vec3f cp1,Vec3f cp2 )
{
	Vec3f pos = (cp1+cp2)/2.0f;

	this->mHalfAxe = cp1-pos;

	float h = 0.1;
	this->mPrevPos = this->mPos;
	this->mPos = pos;
	this->mVel = ( this->mPos - this->mPrevPos ) / h ;
}

void Figure::draw()
{
	glPushMatrix();
		glColor4f( ColorA( 1.0f, 0.0f, 0.0f, 1.0f ) );
		gl::translate(this->mPos-this->mHalfAxe);
		Vec3f n = this->mHalfAxe.normalized();
		float x =/*180/3.14159265 **/ math<float>::acos(Vec3f::xAxis().dot(n));

		// from-to constructor
		Quaternion<float> q(Vec3f::yAxis(),n);
		gl::rotate(q);
		gl::drawCylinder(this->mRadius, this->mRadius, 2 * this->mHalfAxe.length() , 12);

		// drawing closing circles
		gl::translate(Vec3f::yAxis()*this->mHalfAxe.length()*2);
		gl::rotate(Vec3f(90,0,0));
		glNormal3f( 0.0f, 1.0f, 0.0f);
		gl::drawSolidCircle(Vec2f(0.0f,0.0f),this->mRadius);
		gl::rotate(Vec3f(-90,0,0));
		gl::translate(Vec3f::yAxis()*this->mHalfAxe.length()*(-2));
		gl::rotate(Vec3f(90,0,0));
		glNormal3f( 0.0f, -1.0f, 0.0f);
		gl::drawSolidCircle(Vec2f(0.0f,0.0f),this->mRadius);
	glPopMatrix();
}


bool Figure::isCollisionWithBall( Vec3f center,float radius )
{
	// checking projections to the proper axes
	// cylinder made bigger with radius of ball
	// first: is it projected on the center axe?
	Vec3f CP1=this->mPos + this->mHalfAxe + this->mHalfAxe.safeNormalized()*radius;
	Vec3f CP2=this->mPos - this->mHalfAxe - this->mHalfAxe.safeNormalized()*radius;
	if ( (this->mHalfAxe.dot(center-CP2) < 0) || (this->mHalfAxe.dot(center-CP1) > 0) )
	{
		return false;
	}
	// second: is it projected on the bounding circle
	//projection to the circle
	Vec3f proj =center-CP2 - this->mHalfAxe.safeNormalized() * this->mHalfAxe.safeNormalized().dot(center-CP2);
	if ( proj.length() > (this->mRadius + radius) )
	{
		return false;
	}


	return true;
}

void Figure::setBallVelocityAfterCollsision( Vec3f* vel,Vec3f center, float radius )
{
	// TODO missing projection to center axis!!!!!!!!!!!!!!
	// !!MUCH better shape dependent collision
	//*vel = *vel * ((mass - this->mMass)/( mass + this->mMass)) + this->mVel * ((this->mMass*2)/( mass + this->mMass));
	
	// get projection on center line (only if the collision occurs on the "side")
	float len = (center - this->mPos).dot(this->mHalfAxe.safeNormalized());
	if( len>(-this->mHalfAxe.length()) && len<this->mHalfAxe.length())
	{
		Vec3f ray = (this->mPos + this->mHalfAxe.safeNormalized()*len - center);
		float penalty =ray.length();
		float scale=((radius + this->mRadius)/penalty);//*((radius + this->mRadius)/penalty);
		ray = ray.safeNormalized();
		float l=ray.dot(this->mVel);
		l = l>0?l:-l;
		*vel = *vel - ray*(ray.dot(*vel))*2 - ray*l*scale;	
	}
	else
	{

		if( len>(this->mHalfAxe.length()) )
		{
			Vec3f ray = this->mHalfAxe.safeNormalized();
			float penalty =(center-this->mPos).length();
			float scale=((radius + this->mHalfAxe.length())/penalty);//*((radius + this->mHalfAxe.length())/penalty);
			*vel = *vel - ray*ray.dot(*vel)*2 + ray*(ray.dot(this->mVel))*scale;
		}
		else
		{
			Vec3f ray = - this->mHalfAxe.safeNormalized();
			float penalty =(center-this->mPos).length();
			float scale=((radius + this->mHalfAxe.length())/penalty);//*((radius + this->mHalfAxe.length())/penalty);
			*vel = *vel - ray*ray.dot(*vel)*2 + ray*ray.dot(this->mVel)*scale;
		}
	}

}


MenFigure::MenFigure()
{

}

MenFigure::~MenFigure()
{

}

void MenFigure::setup(std::vector<float>radiusParams)
{
	for (unsigned int i = 0; i < radiusParams.size() ; i++)
	{
		Figure fig;
		fig.setup(radiusParams[i]);
		this->bodyParts.push_back( fig );
	}

	this->partNum = radiusParams.size();
}

void MenFigure::update( float* x, float * y, float *z, int n )
{
	//supposing two coordinates
	// not the best way...
	if (n != this->partNum) return;

	for(int i = 0 ; i < n ; ++i)
	{
		this->bodyParts[i].update(Vec3f(x[2*i],y[2*i],z[2*i]),Vec3f(x[2*i + 1],y[2*i + 1],z[2*i + 1]));
	}

}

void MenFigure::draw()
{
	std::vector<Figure>::iterator it = this->bodyParts.begin();
	for ( ; it != this->bodyParts.end(); it++)
	{
		it->draw();
	}
}

bool MenFigure::isCollisionWithBall( Vec3f center,float radius )
{
	for ( unsigned int i = 0; i < this->bodyParts.size(); i++)
	{
		if (this->bodyParts[i].isCollisionWithBall(center,radius))
		{
			collisionID = i;
			return true;
		}
	}
	return false;

}

void MenFigure::setBallVelocityAfterCollsision( Vec3f* vel,Vec3f center,float radius )
{
	// the first collision dominates.... BAD
	this->bodyParts[this->collisionID].setBallVelocityAfterCollsision(vel,center,radius);
	return;
}
