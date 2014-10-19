#ifndef __RENDERER__
#define __RENDERER__

#include "scene.h"
#include "image.h"

#define COEF 30

#include "TrackProgress.h"

TrackProgress * GetRendererTrack();

class Renderer
{
protected:
	Scene * _scene;
	Camera * _camera;
	Image * _image;
	void Destroy();
public:
	Renderer();
	virtual void Init(Scene * scene, Image * image, Camera * camera);
	virtual void Render( );
	virtual Vector4d RenderPixel( const int &x, const int &y ) = 0;
	virtual ~Renderer();
};

#endif