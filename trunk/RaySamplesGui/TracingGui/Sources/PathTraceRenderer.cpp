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
		if ( _bouncer->Stop(ray,isec,throughput) ) 
			break; 

		ray = _bouncer->Bounce(ray, isec);
		isec.t = FLT_MAX;
		if ( !_scene->FindIntersection(ray,isec) )
		{
			//DoAssert(false);
			break;
		}

		// calculate indirect light
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
	if ( _renderMask & RIndirectMask )
	{
		if (_renderMask & RIndirectLightBounced )
			_bouncer = new FiniteBouncer(bounces);
		else 
		{
			throw "Not implemented";
		}
	}
}
