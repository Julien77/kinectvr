#include "room.h"
#include "Resources.h"

#include "cinder/app/AppBasic.h"

#define ABS_DIFF(a,b) ((a-b)>0)?(a-b):(b-a)



Room::Room()
{
    minX = 0;
    maxX = 100;
    minY = 0;
    maxY = 100;
    minZ = 0;
    maxZ = 100;
	
}

Room::~Room()
{
	// no dynamic memory to free, nothing to do (so far)
}

void Room::setup(Vec3f cornerOne, Vec3f cornerTwo)
{
	//namespace 
	using namespace ci::app;

	// initializing every corner. 
	// Call them bottom1, bottom2, bottom3, bottom4
	// with bottom1=cornerOne (with counter clockwise ordering)
	// and top1, top2, top3, top4 with top3=cornerTwo
	// 			  ____ corner two			
	//  		 /___/|
	// 			| |_|_|
	// 			|/__|/
	//  corner one

	//with bottom numbers (same for top)
	// 			  ____ corner two
	//  		 /___/|
	// 			|4|_|_|3
	// 			|/__|/
	//			1	2

	// it IMPORTANT that the x,y axes form a regular cartesian coordinate system 
	// but the z axis points towards the viewer (positive values)

	GLfloat width=ABS_DIFF(cornerOne.x,cornerTwo.x);
	GLfloat depth=ABS_DIFF(cornerOne.z,cornerTwo.z);

	this->mbottom1 = cornerOne;
	this->mbottom2 = cornerOne + Vec3f(width,0,0);
	this->mbottom3 = cornerOne + Vec3f(width,0,-depth);
	this->mbottom4 = cornerOne + Vec3f(0,0,-depth);

	this->mtop1 = cornerTwo + Vec3f(-width,0, depth);
	this->mtop2 = cornerTwo + Vec3f(0,0, depth);
	this->mtop3 = cornerTwo ;
	this->mtop4 = cornerTwo + Vec3f(-width,0,0);



	// setting up textures. It should be later changed...
	//this->mFloorUrl = Url( "http://fc02.deviantart.net/fs44/f/2009/134/f/a/Stone_Dungeon_Floor_by_pfunked.jpg" );
	//this->mFloorTex = gl::Texture( loadImage( loadUrl( this->mFloorUrl) ) );
	this->mFloorTex = gl::Texture( loadImage( loadResource( RES_IMAGE1 ) ) );
    //this->mWallUrl= Url("http://www.publicdomainpictures.net/pictures/8000/velka/brick-wall-with-painted-sign-108112778351653zU7.jpg");
	//this->mWallTex  = gl::Texture( loadImage( loadUrl( this->mWallUrl ) ) );
    this->mWallTex  = gl::Texture( loadImage( loadResource( RES_IMAGE2 ) ) );


	

	// setup camera parameters
	this->mCamZ = 0.0f;   //< these should NOT be hardcoded!!!
	this->mCamX = 0.0f;
	this->mCamY = 0.5f * (this->mtop1.y - this->mbottom1.y)  +   this->mbottom1.y;
	this->mEye			= Vec3f( mCamX, mCamY, mCamZ );
	this->mCenter			=Vec3f( mCamX, mCamY, mCamZ+10 );		//< looking in the direction of the z axis
	this->mUp				= Vec3f::yAxis();						//< controlling the angle of the horizon..
	
	// setting perspective view
	this->mCam.setPerspective( 75.0f, getWindowAspectRatio(), 5.0f, 2000.0f ); 



	// setup the parameters of the small display on the left
	mParams = params::InterfaceGl( "Kinect", Vec2i( 200, 160 ) );
	mParams.addParam( "Scene Rotation", &mSceneRotation, "opened=1" );
	mParams.addSeparator();
	mParams.addParam( "CamPos Z", &mCamZ, "min=-500.0 max=500.0 step=2.0 keyIncr=s keyDecr=w" );
	mParams.addParam( "CamPos X", &mCamX, "min=-500.0 max=500.0 step=2.0 keyIncr=a keyDecr=d" );
	mParams.addParam( "CamPos Y", &mCamY, "min=-500.0 max=500.0 step=2.0 keyIncr=r keyDecr=f" );


	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);   //<- this call should enable lighting to effect our texture
	
	// enabling global lighting and light source number 0
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	

	// you can enable 4 (at least) different kinds of lights
	// we use only two
	bool DIFFUSE		= true;
	bool AMBIENT		= true;
	bool SPECULAR		= false;
	bool EMISSIVE		= false;

	// light properties
	// looks like it is not necessary to declare them as global variables
	GLfloat  no_mat[]			= { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat  mat_ambient[]		= { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat  mat_diffuse[]		= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat  mat_specular[]		= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat  mat_emission[]		= { 0.0f, 0.1f, 0.3f, 0.0f };

	GLfloat  mat_shininess[]	= { 128.0f };
	GLfloat  no_shininess[]		= { 0.0f };
	GLfloat  light_position[]	= { 0.0f, 0.5f * (this->mtop1.y - this->mbottom1.y)  +   this->mbottom1.y, 0.0f, 1.0f };

	// setting the position of the light source
	glLightfv( GL_LIGHT0, GL_POSITION, light_position );

	if( DIFFUSE ){
		// ?? I don't exactly know what is this. Some property of the materials
		// reacting with light
		ci::ColorA color( CM_HSV, 1.0f, 0.0f, 1.0f, 1.0f );
		glMaterialfv( GL_FRONT, GL_DIFFUSE,	color );
		
		// setting the color of diffuse light
		glLightfv(GL_LIGHT0, GL_DIFFUSE, mat_diffuse);
	} else {
		glMaterialfv( GL_FRONT, GL_DIFFUSE,	no_mat );
	}

	if( AMBIENT )
	{
		glMaterialfv( GL_FRONT, GL_AMBIENT,	mat_ambient );
		// setting the color of ambient light
		glLightfv(GL_LIGHT0, GL_AMBIENT, mat_ambient);
	}
	else
		glMaterialfv( GL_FRONT, GL_AMBIENT,	no_mat );

	if( SPECULAR ){
		glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
		glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess );
		// setting the color of specular light
		glLightfv(GL_LIGHT0, GL_SPECULAR, mat_specular);
	} else {
		glMaterialfv( GL_FRONT, GL_SPECULAR, no_mat );
		glMaterialfv( GL_FRONT, GL_SHININESS, no_shininess );
	}

	if( EMISSIVE )
	{
		glMaterialfv( GL_FRONT, GL_EMISSION, mat_emission );
		// setting the color of emissive light
		glLightfv(GL_LIGHT0, GL_EMISSION, mat_emission);
	}
	else
		glMaterialfv( GL_FRONT, GL_EMISSION, no_mat );			

}

void Room::update(Vec3f eye, Vec3f center, Vec3f up)
{
    
    	
	// TODO check camera prooperies not to be able to "leave the room"


	// just until we use the keyboard for testing
	if(eye== Vec3f(0,0,0) && center== Vec3f(0,0,0) && up== Vec3f(0,0,0))
	{
		// UPDATE CAMERA
		this->mEye = Vec3f( this->mCamX, this->mCamY, this->mCamZ );;
		this->mCenter	=Vec3f( this->mCamX, this->mCamY, this->mCamZ ) - this->mSceneRotation.v;
		this->mCam.lookAt( this->mEye, this->mCenter, this->mUp );
	}
	else
	{
		this->mCam.lookAt( eye, center, up );	
	}

	//gl::setMatrices( this->mCam );
	//gl::rotate( mSceneRotation*(-1) );
}

void Room::draw()
{
    glPushMatrix();
    
	glEnable( GL_TEXTURE_2D );		//< Enabling texture
	this->mWallTex.bind();			//< setting the image for the walls

	//Drawing the rectangles for walls
	glBegin( GL_QUADS );

	// y-z plane
	//left
	glNormal3f( 1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); 
	glVertex3f( this->mbottom1 );
	glTexCoord2f(1.0f, 0.0f); 
	glVertex3f( this->mbottom4 );
	glTexCoord2f(1.0f, 1.0f); 
	glVertex3f( this->mtop4 );
	glTexCoord2f(0.0f, 1.0f); 
	glVertex3f( this->mtop1);

	//right
	glNormal3f( -1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); 
	glVertex3f( this->mbottom2 );
	glTexCoord2f(1.0f, 0.0f); 
	glVertex3f( this->mbottom3);
	glTexCoord2f(1.0f, 1.0f); 
	glVertex3f( this->mtop3 );
	glTexCoord2f(0.0f, 1.0f); 
	glVertex3f( this->mtop2);

	//x-y plane
	//front
	glNormal3f( 0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); 
	glVertex3f( this->mbottom1 );
	glTexCoord2f(1.0f, 0.0f); 
	glVertex3f( this->mbottom2 );
	glTexCoord2f(1.0f, 1.0f); 
	glVertex3f( this->mtop2 );
	glTexCoord2f(0.0f, 1.0f); 
	glVertex3f( this->mtop1 );

	glColor4f( ColorA( 0.8f, 0.8f, 0.6f, 1.0f ) );
	//back
	glNormal3f( 0.0f, 0.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); 
	glVertex3f( this->mbottom3 );
	glTexCoord2f(1.0f, 0.0f); 
	glVertex3f( this->mbottom4 );
	glTexCoord2f(1.0f, 1.0f); 
	glVertex3f( this->mtop4 );
	glTexCoord2f(0.0f, 1.0f); 
	glVertex3f( this->mtop3 );

	glEnd();

	// using different texture for floor
	// we have to be outside of glBegin otherwise it has no effect
	this->mFloorTex.bind();

	glBegin( GL_QUADS );
	glNormal3f( 0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); 
	glVertex3f( this->mbottom1 );
	glTexCoord2f(1.0f, 0.0f); 
	glVertex3f( this->mbottom2 );
	glTexCoord2f(1.0f, 1.0f); 
	glVertex3f( this->mbottom3 );
	glTexCoord2f(0.0f, 1.0f); 
	glVertex3f( this->mbottom4);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glBegin( GL_QUADS );
	// drawing top with a color (later could be changed to some texture)
	//top, yellowish????????
	glColor4f( ColorA( 1.0f, 1.0f, 0.9f, 1.0f ) );
	glNormal3f( 0.0f, -1.0f, 0.0f);
	glVertex3f( this->mtop1 );
	glVertex3f( this->mtop2 );
	glVertex3f( this->mtop3 );
	glVertex3f( this->mtop4 );
	glEnd();

	

		// DRAW PARAMS WINDOW
//	params::InterfaceGl::draw();
    
    glPopMatrix();
}

void Room::convertCoord(Vec3f *vec)
{
    vec->x = (vec->x - minX)/(maxX - minX)*this->mtop2.x;
    vec->y = (vec->y - minY)/(maxY - minY)*this->mtop1.y;
    vec->z = (vec->z - minZ)/(maxZ - minZ)*this->mtop1.z;
}

void Room::convertCoord(float* x,float* y,float* z, int n)
{
	for(int i=0; i < (2*n);++i)
	{
		x[i] = (x[i] - minX)/(maxX - minX)*this->mtop2.x;
		y[i] = (y[i] - minY)/(maxY - minY)*this->mtop1.y;
		z[i] = (z[i] - minZ)/(maxZ - minZ)*this->mtop1.z;
	}
	
}


bool Room::isCollisionWithBall( Vec3f center,float radius )
{
	if (center.y < (this->mbottom1.y + radius))
	{
		return true;
	}
	if (center.y > (this->mtop1.y - radius))
	{
		return true;
	}
	if (center.x < (this->mbottom1.x + radius))
	{
		return true;
	}
	if (center.x > (this->mbottom2.x -radius))
	{
		return true;
	}
	// z axis has a direction to the viewer
	if (center.z > (this->mbottom1.z - radius))
	{
		return true;
	}
	if (center.z < (this->mbottom3.z + radius))
	{
		return true;
	}

	return false;
}

void Room::setBallVelocityAfterCollision( Vec3f* vel,Vec3f* center,float radius )
{
	float dissipation=0.75f;

	if (center->y < (this->mbottom1.y + radius))
	{
		center->y = this->mbottom1.y + radius;
		vel->y *=-dissipation;
	}
	if (center->y > (this->mtop1.y - radius))
	{
		center->y=this->mtop1.y - radius;
		vel->y *=-dissipation;
	}
	if (center->x < (this->mbottom1.x + radius))
	{
		center->x=this->mbottom1.x + radius;
		vel->x *=-dissipation;
	}
	if (center->x > (this->mbottom2.x -radius))
	{
		center->x=this->mbottom2.x - radius;
		vel->x *=-dissipation;
	}
	// z axis has a direction to the viewer
	if (center->z > (this->mbottom1.z - radius))
	{
		center->z= this->mbottom1.z - radius;
		vel->z *=-dissipation;
	}
	if (center->z < (this->mbottom4.z + radius))
	{
		center->z=this->mbottom4.z + radius;
		vel->z *=-dissipation;
	}
}
