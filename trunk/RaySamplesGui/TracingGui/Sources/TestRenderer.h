#ifndef __TEST_RENDERER__
#define __TEST_RENDERER__

#include "renderer.h"

class TestRenderer : public Renderer
{
	int _bounces;
public:	
	virtual void Init(Scene * scene, Image * image, Camera * camera);	
	virtual void Render();
};

#endif