#ifndef __PATHRENDERER__
#define __PATHRENDERER__

#include "renderer.h"


class PathTraceRenderer : public Renderer
{
	typedef Renderer base;

  int _bounces;
public:
	PathTraceRenderer();
	virtual Vector4d RenderPixel( const int &x, const int &y );
  void Init( Scene * scene, Image * image, int maxBounces );

private:
	Vector4d RayTrace(Ray ray);
	Vector4d SampleLight(Ray & ray, Intersection & section);
  Vector4d SampleLightBrdf(const Ray & ray, const Intersection & isec);
  Vector4d TrackShine( const Intersection & lightSection, const Intersection & testSection );
  Vector4d SampleMIS(const Ray & ray, const Intersection & isec);
  Vector4d SampleIndirect(const Ray & incomingRay, const Intersection & isec);
};

#endif