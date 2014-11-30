#include "PathTraceRenderer.h"

#include "Debug.h"
#include "RandomNumber.h"

#define MAX_BOUNCES 10

Vector4d PathTraceRenderer::SampleLight(Ray & ray, Intersection & isec)
{
	Vector4d total (0,0,0,0);

	// sample direct light
	for (int iLight =0; iLight< _scene->Lights(); iLight++)
	{
		Geometry * light = _scene->GetLight(iLight);
		if ( light == isec.model )
		{
			total += isec.model->GetMaterial()->Emmisive();
			continue;
		}
		Vector4d lightVector;
		float t;
		Vector4d illumination = light->SampleIllumination( isec, lightVector,t);
		Vector4d outputVector = lightVector;		
		Intersection occSec;
		occSec.t = t - EPSILON;
		Ray r2;
		r2.origin = isec.worldPosition;
		r2.direction = lightVector;
		bool occluded = _scene->FindIntersection(r2, occSec);

		// we want to know if the intersection before
		if ( occluded && ( fabs(occSec.t - t )> EPSILON ) )
			continue;

		Vector4d brdf = isec.model->GetMaterial()->EvalBrdf(-ray.direction, isec.nrm, outputVector);
		for ( int xx = 0; xx < 3; xx++)
		{
			DoAssert(illumination[xx] >=0);
		}
		total += brdf.MultiplyPerElement(illumination);		
	}
	return total;
}

// bounce logic
Vector4d PathTraceRenderer::Bounced(Ray & xray, Intersection & xsection )
{
	Vector4d total(0,0,0,0);
	Vector4d throughput(1,1,1,1);
	Ray ray = xray;
	Intersection isec = xsection;
  while (true)
	{
    float pdf;
    // we can always bounce, ray is now bounced ray
    if (!_bouncer->Bounce(ray, isec,throughput, pdf))
      break;

    isec.t = FLT_MAX;
    if ( !_scene->FindIntersection(ray,isec) )
    {
      //DoAssert(false);
      break;
    }

		// calculate indirect light
    // this is shadow ray
		total += SampleLight(ray,isec).MultiplyPerElement(throughput);
	}
	return total;
}

Vector4d PathTraceRenderer::RayTrace(Ray ray)
{
	Vector4d total;
	total.Zero();

	// first hit is common
	Intersection isec;
	if ( !_scene->FindIntersection(ray,isec) )
	{
		return BLACK;
	}

	if ( _renderMask & RIndirectLightBounced)
		total += Bounced(ray,isec);

	if ( _renderMask & RDirectLight )
		total += SampleLight(ray,isec);
  if ( _renderMask & RDirectBRDF )
    total += SampleLightBrdf(ray,isec);

#if 0 && _DEBUG
	float xx = total.Size2();
	DoAssert(xx > 0);
#endif
	return total;
}

Vector4d PathTraceRenderer::RenderPixel(const int &x, const int &y)
{
	// we start at camera
	// int u = 56,v = 56;
	if (_bouncer)
		_bouncer->Init();
	Ray ray = _scene->GetRay( x + GetFloat(), y + GetFloat() );
	return RayTrace( ray );
}

PathTraceRenderer::PathTraceRenderer() : Renderer(), _bouncer(NULL)
{

}

void PathTraceRenderer::Init(Scene * scene, Image * image, int bounces)
{
	base::Init(scene,image,bounces);
	if ( _renderMask & RIndirectLightBounced )
	{
			_bouncer = new FiniteBouncer(bounces);
	}
}

Vector4d PathTraceRenderer::SampleLightBrdf(const Ray & ray, const Intersection & isec)
{
  // generate the ray according to BRDF
  const Material * m = isec.model->GetMaterial();
  if ( m->IsLight() )
    return Vector4d(1,1,1,1);
  float pdf;
  Vector4d sampledDir = m->SampleBrdf( ray.direction, isec.nrm, pdf );
  if (sampledDir.Size2() == 0) 
    return Vector4d(0,0,0,0); 
  Ray r;
  r.origin = isec.worldPosition;
  r.direction = sampledDir;
  Intersection isec2;
  if ( !_scene->FindIntersection( r, isec2 ) )
    return Vector4d(0,0,0,0); // did not intersect anything
  Vector4d illumination = isec2.model->GetMaterial()->Illumination(sampledDir);
  Vector4d output;
  Vector4d brdf = isec2.model->GetMaterial()->EvalBrdf(sampledDir,isec.nrm,output);
  Vector4d ret = illumination.MultiplyPerElement( brdf );
  return ret;
}
