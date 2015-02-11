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

class PathTraceRenderer : public Renderer
{
	typedef Renderer base;

  int _bounces;
  std::vector<Surfel> _surfels;
public:
	PathTraceRenderer();
	virtual Vector4d RenderPixel( const int &x, const int &y );
  void Init( Scene * scene, Image * image, int maxBounces );
  void Bake()
  {
    if (_renderCtx.mask & RGlobalIllumination)
    {
      _surfels.clear();
      // fill surfels
      Geometry * geom;
      for ( int i =0; geom = _renderCtx.scene->Model(i); i++)
      {
        geom->GenerateSurfels(_surfels,0.05);
      }
      // pre-bake direct illumination for each surfel
      for ( int i =0; i < _surfels.size(); i++)
      {

      }
    }
  }
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