#ifndef __RENDERER__
#define __RENDERER__

#include "scene.h"
#include "image.h"

#define COEF 30

#include "TrackProgress.h"

TrackProgress * GetRendererTrack();

#define RNone 0
#define RDirectLight 1
#define RDirectBRDF 2
#define RIndirectOffset 2
#define RIndirectLightBounced ( 1 << RIndirectOffset )
#define RIndirectLightMcBrdf ( 2 << RIndirectOffset )
#define RIndirectLightMcLight ( 4 << RIndirectOffset )
#define RIndirectLightMIS ( 8 << RIndirectOffset )
#define RGlobalIllumination ( 16 << RIndirectOffset )

class Bouncer
{
public:
	// resets 
	virtual void Init() = 0;

	// return new direction
  virtual bool Bounce(Ray & ray, Intersection & section, Vector4d& throughput, float & pdf) = 0;

};

class FiniteBouncer : public Bouncer
{
	int _maxBounces;
	int _counter;
public:
	FiniteBouncer( int bounces );
	void Init();
  bool Bounce( Ray & ray, Intersection & section, Vector4d& throughput, float & pdf);
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