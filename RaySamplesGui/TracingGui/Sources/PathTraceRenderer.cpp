#include "PathTraceRenderer.h"

Vector4d PathTraceRenderer::RenderPixel(const int &x, const int &y, const int & type)
{
	// we start at camera
	Ray ray = _camera->GetRay(x,y);
	Vector4d total;
	total.Zero();
	Intersection isec;
	if ( _scene->FindIntersection(ray,isec) )
		return BLACK;
	
	// sample direct light
	for (int iLight =0; iLight< _scene->Lights(); iLight++)
	{
		Geometry * light = _scene->GetLight(iLight);
		Vector4d outputVector;
		float pdf;
		Vector4d illumination = light->SampleIllumination( isec );
		Vector4d brdf = isec.model->GetMaterial()->EvalBrdf(-ray.direction, outputVector,pdf);
		if (type & RDirectLight)
		{
			total += brdf.Scalar(illumination);
		}
	}


	return total;
}