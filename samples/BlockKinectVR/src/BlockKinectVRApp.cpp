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

#include "room.h"
#include "figure.h"

using namespace ci;
using namespace ci::app;
using namespace std;


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
	Figure mFig;

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
    //Initialize the Kinect
	_manager = V::OpenNIDeviceManager::InstancePtr();
	_device0 = _manager->createDevice( "data/configIR.xml", true );
	if( !_device0 ) 
	{
		DEBUG_MESSAGE( "(App)  Couldn't init device0\n" );
		exit( 0 );
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
    
//    
//    // CAREFUL the second vertex is "behind" the first!!
//	this->mRoom.setup(Vec3f(0,0,0),Vec3f(100,100,-100));
//	this->mFig.setup();

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
        mOneUserTex.update( getUserColorImage(1) );
        
        V::OpenNIUserRef user = _manager->getUser(1);
        float *CoM = new float[3];
        CoM = user->getCenterOfMass();
        console()<< "CoM.x=" << CoM[0] << ", CoM.y=" << CoM[1] << ", Com.z=" << CoM[2] << endl;
        // These are the values just after calibration pose: CoM.x=333.91, CoM.y=232.187, Com.z=873.077

        
//        // calling it with all zeros let you control the camera otherwise it
//        // should take the given position
//        this->mRoom.update(Vec3f(CoM[0],CoM[1],CoM[2]),Vec3f(0,40,-1),Vec3f(0,1,0));
//        
//        // for fun move our "figure"
//        static float t=0;
//        this->mFig.update(Vec3f(30 + 20 * math<float>::sin(t), 30,-40));
//        t += 0.01;
//        
        delete [] CoM;
    }
    
}

void BlockKinectVRAppApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ), true ); 

	float sx = 320/2;
	float sy = 240/2;
	float xoff = 10;
	float yoff = 10;
	glEnable( GL_TEXTURE_2D );
	gl::color( cinder::ColorA(1, 1, 1, 1) );
    gl::draw( mDepthTex, Rectf( xoff, yoff, xoff+sx, yoff+sy) );
    gl::draw( mColorTex, Rectf( xoff+sx*1, yoff, xoff+sx*2, yoff+sy) );
    if( _manager->hasUsers() && _manager->hasUser(1) ) gl::draw( mOneUserTex, Rectf( 640, 0, 1280, 640 )  );//Rectf( 0, 0, 640, 480)
	if( _manager->hasUsers() && _manager->hasUser(1) )
	{
		// Render skeleton if available
		_manager->renderJoints( 3 );

		// Get list of available bones/joints
		// Do whatever with it
		//V::UserBoneList boneList = _manager->getUser(1)->getBoneList();//Doesn't work
        
        // Get the center of mass of the user 1
        V::OpenNIUserRef user = _manager->getUser(1);
        float *CoM = new float[3];
        CoM = user->getCenterOfMass();
        console()<< "CoM.x=" << CoM[0] << ", CoM.y=" << CoM[1] << ", Com.z=" << CoM[2] << endl;
        // These are the values just after calibration pose: CoM.x=333.91, CoM.y=232.187, Com.z=873.077
        // The console says: BlockKinectVR(445,0xa03cb540) malloc: *** error for object 0xe352b8: pointer being freed was not allocated *** set a breakpoint in malloc_error_break to debug
        // Camera is in front of us: x starts at 0 on the left, y starts at 0 on the floor, z starts at O near the camera
        delete [] CoM;
	}
    
//    gl::enableDepthRead();
//	gl::enableDepthWrite();
//	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//    
//	// drawing the components
//	this->mRoom.draw();
//	this->mFig.draw();
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
}

CINDER_APP_BASIC( BlockKinectVRAppApp, RendererGl )
