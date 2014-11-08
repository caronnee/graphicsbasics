#include "PathTraceRenderer.h"

#include "Debug.h"

Vector4d PathTraceRenderer::RenderPixel(const int &x, const int &y, const int & type)
{
	// we start at camera
	Ray ray = _camera->GetRay(x,y);
	Vector4d total;
	total.Zero();

	Intersection isec;
	if ( !_scene->FindIntersection(ray,isec) )
	{
		__debugbreak();
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
		Intersection dummySec;
		dummySec.t = t;
		Ray r2;
		r2.origin = isec.worldPosition + lightVector* EPSILON;
		r2.direction = lightVector;
		bool occluded = _scene->FindIntersection(r2, dummySec);
		if ( occluded && ( (dummySec.nrm.Dot(isec.nrm) < 0) ) )
			continue;
		Vector4d brdf = isec.model->GetMaterial()->EvalBrdf(-ray.direction, isec.nrm, outputVector);
		if (type & RDirectLight)
		{
			total += brdf.MultiplyPerElement(illumination);
		}
	}
	{
		// in the end, try to use also ambient lighting
		Vector4d outputVector;
		Vector4d illumination = _scene->Ambient().SampleIllumination(-ray.direction,isec.nrm, outputVector);
		// in case of ambient lighting we might want to consider only those that do not intersect the same model 9 or nothing
		Ray r2;
		r2.direction = outputVector;
		r2.direction.Normalize();
		r2.origin = isec.worldPosition + r2.direction * EPSILON;
	
		Intersection dummySec;
		dummySec.t = isec.t;
		if ( !_scene->FindIntersection(r2,dummySec) || (dummySec.nrm.Dot(isec.nrm) >= 0) )
		{
			Vector4d brdf = isec.model->GetMaterial()->EvalBrdf(-ray.direction, isec.nrm, outputVector);
			if (type & RDirectLight)
			{
				total += brdf.MultiplyPerElement(illumination);
			}
		}
	}
#if 0 && _DEBUG
	float xx = total.Size2();
	DoAssert(xx > 0);
#endif
	return total;
}