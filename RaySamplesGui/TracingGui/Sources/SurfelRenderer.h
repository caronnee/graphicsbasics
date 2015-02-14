#ifndef __SURFELRENDERER__
#define __SURFELRENDERER__

#include "renderer.h"
#include "DirectMCRenderer.h"
#include <vector>

class SurfelRenderer : public DirectMCRenderer, public Renderer
{
  const Vector4d SampleGlobalIllumination(const Ray & ray, const Intersection & isec);

  static const int coarseSize;
public:

  std::vector<Surfel> _surfels;

  SurfelRenderer( const RenderContext & ctx);

  // initialization of surfels
  virtual void Init ( Image * image );
  virtual void Bake();
  virtual Vector4d RenderPixel( const int &x, const int &y );
  void test();
  // rendering pixels
};

#endif