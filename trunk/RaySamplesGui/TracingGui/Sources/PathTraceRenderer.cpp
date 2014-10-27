#include "PathTraceRenderer.h"

Vector4d PathTraceRenderer::RenderPixel(const int &x, const int &y, const int & type)
{
	// we start at camera
	Ray ray = _camera->GetRay(x,y);
	Vector4d total;
	total.Zero();
	Intersection isec;
	if ( !_scene->FindIntersection(ray,isec) )
		return BLACK;
	
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
		Vector4d outputVector;
		float pdf;
		Vector4d illumination = light->SampleIllumination( isec, sampledDir);
		Intersection i2;
		Ray r2;
		r2.origin = isec.worldPosition;
		r2.direction = sampledDir;
		bool occluded = _scene->FindIntersection(r2, i2);
		if (occluded && (i2.model!=isec.model))
			continue;
		Vector4d brdf = isec.model->GetMaterial()->EvalBrdf(-ray.direction, outputVector,pdf);
		if (type & RDirectLight)
		{
			if (isec.model->Type() == TypeSphere)
			{
				__debugbreak();
			}
			total += brdf.MultiplyPerElement(illumination);
		}
	}


	return total;
}