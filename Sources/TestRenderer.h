#ifndef __TEST_RENDERER__
#define __TEST_RENDERER__

#include "renderer.h"

class TestRenderer : public Renderer
{
public:	
  TestRenderer(const RenderContext & ctx): Renderer(ctx) {}
	virtual void Init(Scene * scene, Image * image, int bounces);
	Vector4d RenderPixel( const int &x, const int &y );
};

#endif