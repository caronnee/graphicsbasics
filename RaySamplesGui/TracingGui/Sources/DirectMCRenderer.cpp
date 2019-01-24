#include "DirectMCRenderer.h"
#include "renderer.h"
#include "Debug.h"

Vector4d DirectMCRenderer::SampleLight(Ray & ray, Intersection & isec)
{
  Vector4d total (0,0,0,0);

  // sample direct light
  for (int iLight =0; iLight< _scene->Lights(); iLight++)
  {
    // sample from light
    Geometry * light = _scene->GetLight(iLight);
    if ( light == isec.model )
    {
      total += isec.model->GetMaterial()->Emmisive();
      continue;
    }

    Vector4d lightVector;
    float t;
    Vector4d illumination = light->SampleIllumination( isec, lightVector,t);
    float pdf = light->GetDirectionalPdf(lightVector,isec.nrm,isec.worldPosition,t);
    if ( pdf < 0 )
      continue;
    Intersection occSec;
    //occSec.t = t - EPSILON;
    occSec.model = light;
    Ray r2;
    r2.origin = isec.worldPosition;
    r2.direction = lightVector;
    bool occluded = _scene->FindIntersection(r2, occSec);

    // we want to know if the intersection before
    if ( occluded )
      continue;
    // there is intersection that 
    Vector4d brdf = isec.model->GetMaterial()->EvalBrdf(-ray.direction, isec.nrm, lightVector);
    for ( int xx = 0; xx < 3; xx++)
    {
      DoAssert(illumination[xx] >=0);
    }
    total += brdf.MultiplyPerElement(illumination)/pdf;		
  }
  return total;
}

DirectMCRenderer::DirectMCRenderer(Scene * scene) : _scene(scene)
{

}

