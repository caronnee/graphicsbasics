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
		Vector4d sampledDir;
		float pdf;
		float t;
		Vector4d illumination = light->SampleIllumination( isec, sampledDir,t);
		Vector4d outputVector = sampledDir;		
		Intersection i2;
		i2.t = t;
		Ray r2;
		r2.origin = isec.worldPosition;
		r2.direction = sampledDir;
		bool occluded = _scene->FindIntersection(r2, i2);
		if ( occluded && (i2.t < t - 0.001f) )
			continue;
		/*if ( isec.model->Type() == TypeSphere )
			__debugbreak();*/
		Vector4d brdf = isec.model->GetMaterial()->EvalBrdf(-ray.direction, isec.nrm, outputVector);
		if (type & RDirectLight)
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