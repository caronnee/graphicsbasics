#ifndef __PATHRENDERER__
#define __PATHRENDERER__

#include "renderer.h"

class PathTraceRenderer : public Renderer
{
public:
	PathTraceRenderer()  : Renderer()
	{

	}
	virtual Vector4d RenderPixel( const int &x, const int &y, const int & type );
protected:
private:
};

#endif