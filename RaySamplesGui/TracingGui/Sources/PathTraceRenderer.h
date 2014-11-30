#ifndef __PATHRENDERER__
#define __PATHRENDERER__

#include "renderer.h"


class PathTraceRenderer : public Renderer
{
	typedef Renderer base;
	Bouncer * _bouncer;
public:
	PathTraceRenderer();
	void Init(Scene * scene, Image * image, int bounces);
	virtual Vector4d RenderPixel( const int &x, const int &y );

private:
	Vector4d RayTrace(Ray ray);
	Vector4d Bounced(Ray & ray, Intersection & section );
	Vector4d SampleLight(Ray & ray, Intersection & section);
  Vector4d SampleLightBrdf(const Ray & ray, const Intersection & isec);
};

#endif