#include "TestRenderer.h"


void TestRenderer::Init(Scene * scene, Image * image, int bounces)
{
	Renderer::Init(image);
}

#if _DEBUG
extern bool stop;
#endif

Vector4d TestRenderer::RenderPixel(const int &x, const int &y)
{
	Ray ray = _renderCtx.scene->GetRay((float)x +0.5,(float)y +0.5);
	Vector4d total = Vector4d(0,0,0,0);
	// so far only ambient
	Intersection isect;
	bool found = _renderCtx.scene->FindIntersection(ray, isect);
	if ( !found )
	{
		//		DoAssert(false);
		return BLACK; //nothing got rendered in this pixel
	}
  const Material* material = isect.model->GetMaterial();
  if (material->IsLight())
    total = BLACK;
  else
  {
    // eval brdf
	  Vector4d output(0,0,0,0);
	  total = isect.model->GetMaterial()->EvalBrdf( -ray.direction, isect.nrm, output);
  }
  return total;
}

