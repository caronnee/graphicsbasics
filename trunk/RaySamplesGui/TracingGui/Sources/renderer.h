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
  XX(IndirectMIS)  \
  XX(GlobalIllumination)  \

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

#define COORDS_ONLY (1 <<31 )

enum DirectMask
{
  DIRECT_LIGHT(CREATE_ENUMS_SHIFT)
};

enum InDirectMask
{
 INDIRECT_LIGHT(CREATE_ENUMS_SHIFT_INDIRECT)
};

// might be later used
class Bouncer
{
public:
	// resets 
	virtual void Init() = 0;

	// return new direction and directional pdf that this was generated with
  virtual bool Bounce(Ray & ray, Intersection & section, Vector4d& throughput) = 0;

};

struct RenderContext
{
  // scene contains also camera
  Scene * scene;
  int mask;
  int start[2];
  int end[2];
  int renderMask;
  int iterations;
  int bounces;
  // fixed points
  int fixed[2];
};

class Renderer
{
protected:
	bool _stop;
	Image * _image;
	void Destroy();
public:
	RenderContext _renderCtx;

	Renderer();
	virtual void Init(Image * image );
	void Render();
	virtual Vector4d RenderPixel( const int &x, const int &y ) = 0;

	virtual ~Renderer();
	void Stop();
  virtual void Bake();
};

#endif