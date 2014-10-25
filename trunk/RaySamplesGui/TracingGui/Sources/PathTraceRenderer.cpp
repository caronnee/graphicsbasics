#include "PathTraceRenderer.h"

Vector4d PathTraceRenderer::RenderPixel(const int &x, const int &y, const int & type)
{
	Ray ray = _camera->GetRay(x,y);
	Vector4d total;
	total.Zero();
	
	

	return total;
}