#ifndef __RENDERER__
#define __RENDERER__

#include "scene.h"
#include "image.h"

#define COEF 30

#include "TrackProgress.h"

TrackProgress * GetRendererTrack();

#define DIRECT_LIGHT( XX )  \
  XX(DirectLight)  \
  XX(DirectBRDF)  \
  XX(DirectUniform)  \
  XX(DirectMIS)  \
  XX(IndirectOffset)  \

#define INDIRECT_LIGHT( XX )  \
  XX(IndirectLightBounced)  \
  XX(IndirectSimple)  \
  XX(IndirectNextEvent)  \

#define RNone 0
#define  CREATE_ENUMS_DOUBLE(name) S##name,
#define  CREATE_ENUMS_SHIFT(name) R##name = (1 << S##name),
#define  CREATE_ENUMS_SHIFT_INDIRECT(name) R##name = (1 << (S##name + SIndirectOffset)),

enum DirectRenderOffset
{
  DIRECT_LIGHT(CREATE_ENUMS_DOUBLE)
};

enum InDirectRenderOffset
{
  INDIRECT_LIGHT(CREATE_ENUMS_DOUBLE)
};

enum DirectMask
{
  DIRECT_LIGHT(CREATE_ENUMS_SHIFT)
};

enum InDirectMask
{
 INDIRECT_LIGHT(CREATE_ENUMS_SHIFT_INDIRECT)
};

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