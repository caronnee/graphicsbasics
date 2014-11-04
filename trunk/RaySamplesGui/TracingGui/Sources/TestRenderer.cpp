#include "TestRenderer.h"


void TestRenderer::Init(Scene * scene, Image * image, Camera * camera)
{
	Renderer::Init(scene,image,camera);
	_bounces = 1 + COEF *0;
}

#if _DEBUG
extern bool stop;
#endif

Vector4d TestRenderer::RenderPixel(const int &x, const int &y, const int & type)
{
#if _DEBUG
	if ( (x == 34) && (y == 63) )
		stop = true;
	else
		stop = false;
#endif
	Ray ray = _camera->GetRay((float)x +0.5,(float)y +0.5);
	Vector4d total = Vector4d(0,0,0,0);
	// so far only ambient
	Intersection isect;
	bool found = _scene->FindIntersection(ray, isect);
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
	  Vector4d output;
	  total = isect.model->GetMaterial()->EvalBrdf( -ray.direction, isect.nrm, output);
  }
  return total;
}

