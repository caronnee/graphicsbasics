#include "PathTraceRenderer.h"

#include "Debug.h"
#include "RandomNumber.h"

#define MAX_BOUNCES 10

Vector4d PathTraceRenderer::RayTrace(const Ray ray)
{
	Vector4d total;
	total.Zero();

	Intersection isec;
	if ( !_scene->FindIntersection(ray,isec) )
	{
		return BLACK;
	}

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
		if ( _renderMask & RDirectLight )
		{
			total += brdf.MultiplyPerElement(illumination);
		}
	}
#if 0 && _DEBUG
	float xx = total.Size2();
	DoAssert(xx > 0);
#endif
	return total;
}

Vector4d PathTraceRenderer::RenderPixel(const int &x, const int &y)
{
	// we start at camera
//	int u = 44,v = 89;
	Ray ray = _scene->GetRay( x + GetFloat(), y + GetFloat() );
	
	return RayTrace( ray );
}

PathTraceRenderer::PathTraceRenderer() : Renderer()
{

}
