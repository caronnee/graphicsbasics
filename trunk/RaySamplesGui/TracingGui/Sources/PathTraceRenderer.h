#ifndef __PATHRENDERER__
#define __PATHRENDERER__

#include "renderer.h"

struct Generated
{
  Vector4d radiance;
  Vector4d bounded;
  Vector4d brdf;
  float pdf;
};

class SurfelRenderer : public Renderer
{
public:
  static const int coarseSize;

  std::vector<Surfel> &_surfels;
  SurfelRenderer( std::vector<Surfel>& surfels, const RenderContext & ctx);

  // initialization of surfels
  virtual void Init ( Image * image );

  virtual Vector4d RenderPixel( const int &x, const int &y );
  // rendering pixels
  void Bake();
};


class PathTraceRenderer : public Renderer
{
	typedef Renderer base;

  int _bounces;
  std::vector<Surfel> _surfels;
public:
	PathTraceRenderer();
	virtual Vector4d RenderPixel( const int &x, const int &y );
  void Init( Scene * scene, Image * image, int maxBounces );
  void Bake();
private:
  
	Vector4d RayTrace(Ray ray);
	Vector4d SampleLight(Ray & ray, Intersection & section);
  Vector4d SampleLightBrdf(const Ray & ray, const Intersection & isec);
  Vector4d TrackShine( const Intersection & lightSection, const Intersection & testSection );
  Generated SampleMIS(const Ray & ray, const Intersection & isec);
  Vector4d SampleIndirect(const Ray & incomingRay, const Intersection & isec);
  const Vector4d SampleGlobalIllumination(const Ray & ray, const Intersection & isec);
};

#endif