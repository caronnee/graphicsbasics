#ifndef __PATHRENDERER__
#define __PATHRENDERER__

#include "renderer.h"
#include "DirectMCRenderer.h"

struct Generated
{
  Vector4d radiance;
  Vector4d bounded;
  Vector4d brdf;
  float pdf;
};

class PathTraceRenderer : public DirectMCRenderer, public Renderer
{
	typedef Renderer base;

  int _bounces;
public:
	PathTraceRenderer( const RenderContext & ctx);
	virtual Vector4d RenderPixel( const int &x, const int &y );
  void Init( Scene * scene, Image * image, int maxBounces );
  void Bake();
private:
  
	Vector4d RayTrace(Ray ray);
  Vector4d SampleLightBrdf(const Ray & ray, const Intersection & isec);
  Vector4d TrackShine( const Intersection & lightSection, const Intersection & testSection );
  Generated SampleMIS(const Ray & ray, const Intersection & isec);
  Vector4d SampleIndirect(const Ray & incomingRay, const Intersection & isec);
};

#endif