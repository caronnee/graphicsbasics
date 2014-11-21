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
	RGlobalIllumination = 1 << 5,
	RTotal = (1 << 6) 
};

#define RIndirectMask (RTotal - 2)

class Bouncer
{
public:
	// resets 
	virtual void Init() = 0;

	// return new direction
	virtual Ray Bounce(Ray & ray, Intersection & section) = 0;

	// checks whether the bouncing should stop or not
	virtual bool Stop(Ray & ray, Intersection & section, Vector4d & throughput) = 0;
};

class FiniteBouncer : public Bouncer
{
	int _maxBounces;
	int _counter;
public:
	FiniteBouncer( int bounces );
	void Init();
	bool Stop(Ray & ray, Intersection & section, Vector4d & throughput);
	Ray Bounce(Ray & ray, Intersection & section);
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
	virtual void Init(Scene * scene, Image * image, int maxBounces );
	void Render(int iterations);
	virtual Vector4d RenderPixel( const int &x, const int &y ) = 0;

	virtual ~Renderer();
	void Stop();
};

#endif