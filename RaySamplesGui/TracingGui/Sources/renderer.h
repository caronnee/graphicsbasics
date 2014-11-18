#ifndef __RENDERER__
#define __RENDERER__

#include "scene.h"
#include "image.h"

#define COEF 30

#include "TrackProgress.h"

TrackProgress * GetRendererTrack();

enum RenderingTypeMask
{
	RNone = 0,
	RDirectLight = 1,
	RIndirectLightBounced = 1 << 1,
	RIndirectLightMcBrdf = 1 << 2,
	RIndirectLightMcLight = 1 << 3,
	RIndirectLightMIS = 1 << 4,
	RGlobalIllumination = 1 << 5
};

class Renderer
{
protected:
	bool _stop;
	Scene * _scene;
	Image * _image;
	void Destroy();
public:
	int _renderMask;

	Renderer();
	virtual void Init(Scene * scene, Image * image);
	void Render(int iterations);
	virtual Vector4d RenderPixel( const int &x, const int &y ) = 0;

	virtual ~Renderer();
	void Stop();
};

#endif