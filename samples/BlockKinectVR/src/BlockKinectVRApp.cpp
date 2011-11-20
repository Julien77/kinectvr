/***
	SkeletonApp

	A sample app showing skeleton rendering with the kinect and openni.
	This sample renders only the user with id=1. If user has another id he won't be displayed.
	You may change that in the code.

	V.
***/

#include "cinder/app/AppBasic.h"
#include "cinder/imageio.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "VOpenNIHeaders.h"
#include "VOpenNIBone.h"
#include "VOpenNIDevice.h"


#include "room.h"
#include "figure.h"
#include "ball.h"
#include "Sound.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace V;

#define BODY_PARTS_NUM 18

class ImageSourceKinectColor : public ImageSource 
{
public:
	ImageSourceKinectColor( uint8_t *buffer, int width, int height )
		: ImageSource(), mData( buffer ), _width(width), _height(height)
	{
		setSize( _width, _height );
		setColorModel( ImageIo::CM_RGB );
		setChannelOrder( ImageIo::RGB );
		setDataType( ImageIo::UINT8 );
	}

	~ImageSourceKinectColor()
	{
		// mData is actually a ref. It's released from the device. 
		/*if( mData ) {
			delete[] mData;
			mData = NULL;
		}*/
	}

	virtual void load( ImageTargetRef target )
	{
		ImageSource::RowFunc func = setupRowFunc( target );

		for( uint32_t row	 = 0; row < _height; ++row )
			((*this).*func)( target, row, mData + row * _width * 3 );
	}

protected:
	uint32_t					_width, _height;
	uint8_t						*mData;
};


class ImageSourceKinectDepth : public ImageSource 
{
public:
	ImageSourceKinectDepth( uint16_t *buffer, int width, int height )
		: ImageSource(), mData( buffer ), _width(width), _height(height)
	{
		setSize( _width, _height );
		setColorModel( ImageIo::CM_GRAY );
		setChannelOrder( ImageIo::Y );
		setDataType( ImageIo::UINT16 );
	}

	~ImageSourceKinectDepth()
	{
		// mData is actually a ref. It's released from the device. 
		/*if( mData ) {
			delete[] mData;
			mData = NULL;
		}*/
	}

	virtual void load( ImageTargetRef target )
	{
		ImageSource::RowFunc func = setupRowFunc( target );

		for( uint32_t row = 0; row < _height; ++row )
			((*this).*func)( target, row, mData + row * _width );
	}

protected:
	uint32_t					_width, _height;
	uint16_t					*mData;
};


class BlockKinectVRAppApp : public AppBasic 
{
public:
	static const int WIDTH = 1280;
	static const int HEIGHT = 720;

	static const int KINECT_COLOR_WIDTH = 640;	
	static const int KINECT_COLOR_HEIGHT = 480;	
	static const int KINECT_COLOR_FPS = 30;	
	static const int KINECT_DEPTH_WIDTH = 640;
	static const int KINECT_DEPTH_HEIGHT = 480;
	static const int KINECT_DEPTH_FPS = 30;
    
    Room mRoom;
	MenFigure mFig;
	Ball mBall;
    bool mPOV;
	bool mFixed;
    Sound mSound;

    void prepareSettings(Settings* settings);
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	void keyDown( KeyEvent event );

	ImageSourceRef getColorImage()
	{
		// register a reference to the active buffer
		uint8_t *activeColor = _device0->getColorMap();
		return ImageSourceRef( new ImageSourceKinectColor( activeColor, KINECT_COLOR_WIDTH, KINECT_COLOR_HEIGHT ) );
	}

	ImageSourceRef getUserColorImage( int id )
	{
		V::OpenNIUserRef user = _manager->getUser(id);

		// register a reference to the active buffer
		uint8_t *activeColor = user->getPixels();
		return ImageSourceRef( new ImageSourceKinectColor( activeColor, KINECT_COLOR_WIDTH, KINECT_COLOR_HEIGHT ) );
	}

	ImageSourceRef getDepthImage()
	{
		// register a reference to the active buffer
		uint16_t *activeDepth = _device0->getDepthMap();
		return ImageSourceRef( new ImageSourceKinectDepth( activeDepth, KINECT_DEPTH_WIDTH, KINECT_DEPTH_HEIGHT ) );
	} 

	ImageSourceRef getDepthImage24()
	{
		// register a reference to the active buffer
		uint8_t *activeDepth = _device0->getDepthMap24();
		return ImageSourceRef( new ImageSourceKinectColor( activeDepth, KINECT_DEPTH_WIDTH, KINECT_DEPTH_HEIGHT ) );
	}

public:	// Members
	V::OpenNIDeviceManager*	_manager;
	V::OpenNIDevice::Ref	_device0;

	gl::Texture				mColorTex;
	gl::Texture				mDepthTex;
	gl::Texture				mOneUserTex;	 
};

void BlockKinectVRAppApp::prepareSettings( Settings* settings )
{
	settings->setWindowSize( 1280, 480 );
    settings->setFullScreen( false );
    settings->setFrameRate( 30.0f );
}

void BlockKinectVRAppApp::setup()
{
    mPOV = true;
	mFixed = true;
    //Initialize the Kinect
	_manager = V::OpenNIDeviceManager::InstancePtr();
#if  defined(_MSC_VER) || defined(_WIN32)
	//Windows stuff
	_device0 = _manager->createDevice( "../data/configIR.xml", true );
#else
	_device0 = _manager->createDevice( "data/configIR.xml", true );
#endif
	if( !_device0 ) 
	{
		DEBUG_MESSAGE( "(App)  Couldn't init device0\n" );
		//exit( 0 );
	}
    if( _device0)
    {
	_device0->setPrimaryBuffer( V::NODE_TYPE_DEPTH );
	_manager->start();
    }

	gl::Texture::Format format;
	gl::Texture::Format depthFormat;
	mColorTex = gl::Texture( KINECT_COLOR_WIDTH, KINECT_COLOR_HEIGHT, format );
	mDepthTex = gl::Texture( KINECT_DEPTH_WIDTH, KINECT_DEPTH_HEIGHT, format );
	mOneUserTex = gl::Texture( KINECT_DEPTH_WIDTH, KINECT_DEPTH_HEIGHT, format );
    
    // The second vertex is "behind" the first!!
	this->mRoom.setup(Vec3f(0,0,100),Vec3f(100,70,0));
	std::vector<float> rad;
	// BODY_PARTS_NUM parts of body
	for(int i=0;i<BODY_PARTS_NUM;++i)rad.push_back(2.0f);
	this->mFig.setup(rad);
	this->mBall.setup(Vec3f(0,50,50),Vec3f(10,10,10),&(this->mFig),&(this->mRoom));
	//Play sound
	mSound.setup();

}


static void putBones(OpenNIBone* b1, OpenNIBone* b2, float* x,float* y,float* z, int pos)
{
	x[0 + 2*pos] = b1->position[0];
	x[1 + 2*pos] = b2->position[0];
	y[0 + 2*pos] = b1->position[1];
	y[1 + 2*pos] = b2->position[1];
	z[0 + 2*pos] = b1->position[2];
	z[1 + 2*pos] = b2->position[2];
}

static void getProjectiveCoordinates(float* x,float* y,float* z, int pos,V::OpenNIDevice::Ref device)
{
	xn::DepthGenerator* depth = device->getDepthGenerator();
	if( !depth ) return;

	for (int i = 0; i < (2*pos) ; i++)
	{
		XnPoint3D point1;
		XnPoint3D realJointPos1;
		realJointPos1.X = x[i];
		realJointPos1.Y = y[i];
		realJointPos1.Z = z[i];
		depth->ConvertRealWorldToProjective( 1, &realJointPos1, &point1 );
		point1 = realJointPos1;

		x[i] =point1.X;
		y[i] =point1.Y;
		z[i] =point1.Z;
	}
}


void BlockKinectVRAppApp::update()
{	
    // Update textures
    if( _device0)
    {
		mColorTex.update( getColorImage() );
		mDepthTex.update( getDepthImage24() );	// Histogram
    }
    
    // Get texture and coordinates of user
    if( _manager->hasUsers() && _manager->hasUser(1) ) 
    {
        V::OpenNIUserRef user = _manager->getUser(1);
		if (user)
		{
			mOneUserTex.update( getUserColorImage(1) );

			if (mPOV)
			{
				float* CoM = user->getCenterOfMass();
				console()<< "CoM.x=" << CoM[0] << ", CoM.y=" << CoM[1] << ", Com.z=" << CoM[2] << endl;

				//Change volume in function of CoM.z, the parameter sould be inside [0,100]
				mSound.update(CoM[2]/20);


				// calling it with all zeros let you control the camera otherwise it
				// should take the given position
				static Vec3f smoothCenter= Vec3f(0, 0, 0);
				Vec3f newCenter = Vec3f(CoM[0],CoM[1],CoM[2]);
				//convert coordinates to room system
				this->mRoom.convertCoord(&newCenter);
				smoothCenter = smoothCenter* 0.8 + newCenter * 0.2;

				this->mRoom.update(smoothCenter,smoothCenter -Vec3f(0,0,1),Vec3f(0,1,0));
			}
			else
			{
				std::vector<OpenNIBone*> bones = user->getBoneList();
				float * x = new float[2 * BODY_PARTS_NUM];
				float * y = new float[2 * BODY_PARTS_NUM];
				float * z = new float[2 * BODY_PARTS_NUM];
				
				// Getting the coordinates for body parts
				// Always -1 because Head == 1 but bones[0] is HEAD
				
				//renderBone( SKEL_HEAD, SKEL_NECK );
				//renderBone( 1, 2 );
				putBones(bones[0],bones[1],x,y,z,0);
				//renderBone( SKEL_WAIST, SKEL_NECK  );
				//renderBone( 4, 2 );
				putBones(bones[3],bones[1],x,y,z,1);
				//renderBone( SKEL_LEFT_SHOULDER, SKEL_LEFT_ELBOW );
				//renderBone( 6, 7 );
				putBones(bones[5],bones[6],x,y,z,2);
				//renderBone( SKEL_LEFT_ELBOW, SKEL_LEFT_HAND );
				//renderBone( 7, 9 );
				putBones(bones[6],bones[8],x,y,z,3);
				//renderBone( SKEL_RIGHT_SHOULDER, SKEL_RIGHT_ELBOW );
				//renderBone( 12, 13 );
				putBones(bones[11],bones[12],x,y,z,4);
				//renderBone( SKEL_RIGHT_ELBOW, SKEL_RIGHT_HAND );
				//renderBone( 13, 15 );
				putBones(bones[12],bones[14],x,y,z,5);
				//renderBone( SKEL_LEFT_HIP, SKEL_LEFT_KNEE );
				//renderBone( 17, 18 );
				putBones(bones[16],bones[18],x,y,z,6);
				//renderBone( SKEL_LEFT_KNEE, SKEL_LEFT_FOOT );
				//renderBone( 18, 20 );
				putBones(bones[17],bones[19],x,y,z,7);
				//renderBone( SKEL_RIGHT_HIP, SKEL_RIGHT_KNEE );
				//renderBone( 21, 22 );
				putBones(bones[20],bones[21],x,y,z,8);
				//renderBone( SKEL_RIGHT_KNEE, SKEL_RIGHT_FOOT );
				//renderBone( 22, 24 );
				putBones(bones[21],bones[23],x,y,z,9);
				//renderBone( SKEL_NECK, SKEL_LEFT_SHOULDER );
				//renderBone( 2, 6 );
				putBones(bones[1],bones[5],x,y,z,10);
				//renderBone( SKEL_NECK, SKEL_RIGHT_SHOULDER );
				//renderBone( 2, 12 );
				putBones(bones[1],bones[11],x,y,z,11);
				//renderBone( SKEL_LEFT_SHOULDER, SKEL_TORSO );
				//renderBone( 6, 3 );
				putBones(bones[5],bones[2],x,y,z,12);
				//renderBone( SKEL_RIGHT_SHOULDER, SKEL_TORSO );
				//renderBone( 12, 3 );
				putBones(bones[11],bones[2],x,y,z,13);
				//renderBone( SKEL_TORSO, SKEL_LEFT_HIP );
				//renderBone( 3, 17 );
				putBones(bones[2],bones[16],x,y,z,14);
				//renderBone( SKEL_TORSO, SKEL_RIGHT_HIP );
				//renderBone( 3, 21 );
				putBones(bones[2],bones[20],x,y,z,15);
				//renderBone( SKEL_LEFT_HIP, SKEL_RIGHT_HIP );
				//renderBone( 17, 21 );
				putBones(bones[16],bones[20],x,y,z,16);
				//renderBone( SKEL_WAIST, SKEL_TORSO );
				//renderBone( 4, 3 );
				putBones(bones[3],bones[2],x,y,z,17);
				
				//getProjectiveCoordinates(x,y,z,BODY_PARTS_NUM,_device0);

				this->mRoom.convertCoord(x,y,z,BODY_PARTS_NUM);
				this->mFig.update(x,y,z, BODY_PARTS_NUM);
				// normal vec in two steps
				
				//Vec3f normalvec = Vec3f(bones[2]->position[0],bones[2]->position[1],bones[2]->position[2])- Vec3f(bones[20]->position[0],bones[20]->position[1],bones[20]->position[2]);
				//normalvec = normalvec.cross(Vec3f(bones[2]->position[0],bones[2]->position[1],bones[2]->position[2])- Vec3f(bones[3]->position[0],bones[3]->position[1],bones[3]->position[2]));
				//Vec3f up = Vec3f(bones[0]->position[0],bones[0]->position[1],bones[0]->position[2])- Vec3f(bones[1]->position[0],bones[1]->position[1],bones[1]->position[2]);
				// use transformed coords
				Vec3f normalvec = -Vec3f(x[0]-x[4],y[0]-y[4],z[0]-z[4]).cross(Vec3f(x[0]-x[8],y[0]-y[8],z[0]-z[8]));
				Vec3f up = Vec3f(x[0]-x[1],y[0]-y[1],z[0]-z[1]);

				static Vec3f smoothedEye = Vec3f(0,0,0);
				static Vec3f smoothedNormal = Vec3f(0,0,0);
				static Vec3f smoothedUp = Vec3f(0,0,0);
				smoothedEye = smoothedEye * 0.6f + Vec3f(x[0],y[0],z[0]) * 0.4f;
				smoothedNormal = smoothedNormal*0.6f + normalvec *0.4f;
				smoothedUp = smoothedUp*0.6f + up *0.4f;

				console()<<"norm: "<<normalvec<<std::endl;
				if (mFixed)
				{
					this->mRoom.update(Vec3f(50,35,100),Vec3f(50,35,90),Vec3f::yAxis());

				}else
				{
					this->mRoom.update(smoothedEye, 
						Vec3f(x[0],y[0],z[0]) + smoothedNormal.safeNormalized()*10 + Vec3f(0,-2, 0),  //to little bit look down
						smoothedUp);

				}
				

				delete[] x;
				delete[] y;
				delete[] z;
			}
		}
	}

	this->mBall.update(Vec3f(0,0,0),false);
}

void BlockKinectVRAppApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ), true );
    gl::enableDepthRead();
	gl::enableDepthWrite();
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	float sx = 320;
	float sy = 240;
	float xoff = 10;
	float yoff = 10;
	glEnable( GL_TEXTURE_2D );
		gl::translate(Vec3f(0,0,-1));
		gl::color( cinder::ColorA(1, 1, 1, 1) );
		gl::draw( mDepthTex, Rectf( xoff, yoff, xoff+sx, yoff+sy) );
		gl::draw( mColorTex, Rectf( xoff+sx*1, yoff + sy, xoff+sx*2, yoff + 2 * sy) );
		gl::translate(Vec3f(0,0,1));
		//if( _manager->hasUsers() && _manager->hasUser(1) ) gl::draw( mOneUserTex, Rectf( 640, 0, 1280, 640 )  );
		if( _manager->hasUsers() && _manager->hasUser(1) )
		{
			// Render skeleton if available
			_manager->renderJoints( 3 );

			// Get list of available bones/joints
			// Do whatever with it
			//V::UserBoneList boneList = _manager->getUser(1)->getBoneList();//Doesn't work
        
			// The console says: BlockKinectVR(445,0xa03cb540) malloc: *** error for object 0xe352b8: pointer being freed was not allocated *** set a breakpoint in malloc_error_break to debug
			// Camera is in front of us: x starts at 0 on the left, y starts at 0 on the floor, z starts at O near the camera
		}
    glDisable( GL_TEXTURE_2D );
        
	// drawing the components
    glViewport( 640, 0, 640, 460 );

    gl::pushMatrices();
    gl::setMatrices( this->mRoom.mCam );
	this->mRoom.draw();
	this->mFig.draw();
	this->mBall.draw();
    gl::popMatrices();

    glViewport( 0, 0, 1280, 480 ); 
}

void BlockKinectVRAppApp::mouseDown( MouseEvent event )
{
}

void BlockKinectVRAppApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_ESCAPE )
	{
		this->quit();
		this->shutdown();
	}
    else if( event.getChar() == 'f' || event.getChar() == 'F' )
    {
		setFullScreen( ! isFullScreen() );
	}
    else if( event.getChar() == 'i' || event.getChar() == 'I' )
    {
		if( _manager->hasUsers() && _manager->hasUser(1) )
		{
			V::OpenNIUserRef user = _manager->getUser(1);
			if(user)
			{
				float * CoM = user->getCenterOfMass();
				this->mRoom.minX = CoM[0];
				this->mRoom.minY = CoM[1];
				this->mRoom.minZ = CoM[2];
			}
		}
	}
        else if( event.getChar() == 'o' || event.getChar() == 'O' )
    {
		if( _manager->hasUsers() && _manager->hasUser(1) )
		{
			V::OpenNIUserRef user = _manager->getUser(1);
			if(user)
			{
				float * CoM = user->getCenterOfMass();
				this->mRoom.maxX = CoM[0];
				this->mRoom.maxY = CoM[1];
				this->mRoom.maxZ = CoM[2];
			}

		}
	}
        else if( event.getChar() == 'p' || event.getChar() == 'P' )
    {
        mPOV = !mPOV;
    }
	else if( event.getChar() == 'j' || event.getChar() == 'J' )
	{
		mFixed = !mFixed;
	}
	else if( event.getChar() == 'k' || event.getChar() == 'K' )
	{
		if( _manager->hasUsers() && _manager->hasUser(1) )
		{
			V::OpenNIUserRef user = _manager->getUser(1);
			if(user)
			{
				// no error checking
				std::vector<OpenNIBone*> bones = user->getBoneList();
				//renderBone( SKEL_LEFT_KNEE, SKEL_LEFT_FOOT );
				//renderBone( 18, 20 );
				this->mRoom.minX = bones[19]->position[0];
				this->mRoom.minY = bones[19]->position[1];
				this->mRoom.minZ = bones[19]->position[2];

			}
		}
	}
	else if( event.getChar() == 'l' || event.getChar() == 'L' )
	{
		if( _manager->hasUsers() && _manager->hasUser(1) )
		{
			V::OpenNIUserRef user = _manager->getUser(1);
			if(user)
			{
				// no error checking
				std::vector<OpenNIBone*> bones = user->getBoneList();
				//renderBone( SKEL_RIGHT_ELBOW, SKEL_RIGHT_HAND );
				//renderBone( 13, 15 );
				this->mRoom.maxX = bones[14]->position[0];
				this->mRoom.maxY = bones[14]->position[1];
				this->mRoom.maxZ = bones[14]->position[2];
			}

		}
	}
}

CINDER_APP_BASIC( BlockKinectVRAppApp, RendererGl )
