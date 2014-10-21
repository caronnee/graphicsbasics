#include "PathTraceRenderer.h"

Vector4d PathTraceRenderer::RenderPixel(const int &x, const int &y)
{
	Ray ray = _camera->GetRay(x,y);
	Vector4d total;
	total.Zero();
	return total;
}

