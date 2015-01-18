#include "PathTraceRenderer.h"

#include "Debug.h"
#include "RandomNumber.h"

#define MAX_BOUNCES 10
static const int RIndirectMask = (~0) << SIndirectOffset;

Vector4d PathTraceRenderer::SampleLight(Ray & ray, Intersection & isec)
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
		Intersection occSec;
		occSec.t = t - EPSILON;
		Ray r2;
		r2.origin = isec.worldPosition;
		r2.direction = lightVector;
		bool occluded = _scene->FindIntersection(r2, occSec);

		// we want to know if the intersection before
		if ( occluded && ( fabs(occSec.t - t )> EPSILON ) )
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

	if ( _renderMask & RIndirectMask)
		total += SampleIndirect(ray,isec);

	if ( _renderMask & RDirectLight )
		total += SampleLight(ray,isec);
  
  if ( _renderMask & (RDirectBRDF|RDirectUniform) )
  {
    //isec.nrm = Vector4d(0,0,1,0);
    //isec.worldPosition = Vector4d(0,0,-1.25);
    total += SampleLightBrdf(ray,isec);
  }
  if ( _renderMask & RDirectMIS )
    total += SampleMIS(ray,isec);

#if 0 && _DEBUG
	float xx = total.Size2();
	DoAssert(xx > 0);
#endif
	return total;
}

Vector4d PathTraceRenderer::TrackShine( const Intersection & lightSection, const Intersection & testSection )
{
  Vector4d ret(0,0,0,0);
  if (!lightSection.model->GetMaterial()->IsLight())
    return 0;
  // sampleVector
  Vector4d dirToLight = lightSection.worldPosition - testSection.worldPosition;
  // a1, a2;
  float len;
  // todo change
  return lightSection.model->Radiance( testSection.nrm, dirToLight, len);
}

Vector4d PathTraceRenderer::RenderPixel(const int &x, const int &y)
{
	// we start at camera
  int u = x,v=y;
  //u = 133;
  //v = 67;  
  if ( y < 128 )
  {
//    v = 78;  
   // u = 60;
  }
  else
  {
//      v = 87;  
  //  u = 190;
  }
//  u = v = 128;
//  v=250;
  /*Intersection s1,s2; 
  if ( y > 256 )
  {
    v = y - 256;
    Ray r = _scene->GetRay(401,121);
    _scene->FindIntersection(r,s1);
  }
  else
  {
    v=y;
    Ray r = _scene->GetRay(121,121);
    _scene->FindIntersection(r,s1);
  }
  Ray r = _scene->GetRay(x,v);
  _scene->FindIntersection(r,s2);
  return TrackShine(s2,s1);*/
	Ray ray = _scene->GetRay( u,v );
  return RayTrace( ray );
}

PathTraceRenderer::PathTraceRenderer() : Renderer(), _bounces(0)
{

}

Vector4d GCoord;

#include "MathUtil.h"

Vector4d PathTraceRenderer::SampleMIS(const Ray & ray, const Intersection & isec)
{
  const Material * m = isec.model->GetMaterial();
  if ( m->IsLight() )
    return m->Emmisive();

  Vector4d total;
  // number of mis sampling is 1 for start, we might later add some more samples
  float pdf;
  Vector4d sampledDir;
  Vector4d illumination;
  Vector4d brdf;
  int nSamples = 1;
//  if ( false )
  for ( int dummy = 0; dummy < nSamples; dummy++ )
  {
    // sample from one light
    for (int i =0; i < _scene->Lights(); i++)
    {
      float len;
      illumination = _scene->GetLight(i)->SampleIllumination(isec,sampledDir,len);
      // check for occlusion
      Ray r2;
      r2.origin = isec.worldPosition;
      r2.direction = sampledDir;
      Intersection isec2;
      if ( !_scene->FindIntersection(r2,isec2) || fabs(isec2.t - len) < EPSILON )
      {
        float pdf1 = _scene->GetLight(i)->GetDirectionalPdf(sampledDir, isec.nrm, isec.worldPosition, len);
        float pdf2 = isec.model->GetMaterial()->GetDirectionalPdf(sampledDir,isec.nrm);
        brdf = isec.model->GetMaterial()->EvalBrdf(-ray.direction,isec.nrm,sampledDir);
        if ( (pdf1 > 0) && (pdf2 > 0) )
        {
          float misW = 1.0f/(pdf1 + pdf2);
          DoAssert(misW > 0);
          DoAssert(pdf1 > 0);
          DoAssert(pdf2 > 0);
          total += illumination.MultiplyPerElement(brdf) * misW;
        }
      }
    }
  }

  //sample from brdf
  for ( int dummy = 0; dummy < nSamples; dummy++ )
  {
    float pdfBrdf;
    sampledDir = m->SampleBrdf( -ray.direction, isec.nrm, pdfBrdf );
    Ray r2;
    r2.origin = isec.worldPosition;
    r2.direction = sampledDir;
    Intersection isec2;
    if ( (sampledDir.Size2() > 0)  )
    {
      if (_scene->FindIntersection(r2,isec2))
      {
        if (isec2.model->GetMaterial()->IsLight())
        {
          float pdfLight = isec2.model->GetDirectionalPdf(sampledDir, isec.nrm, isec.worldPosition, isec2.t);
          illumination = isec2.model->Radiance(isec.nrm,sampledDir,isec2.t);
          if ( (pdfBrdf > 0) && (pdfLight > 0) )
          {
            float misW = 1.0f / ( pdfBrdf + pdfLight );
            DoAssert(misW > 0);
            DoAssert(pdfBrdf > 0);
            brdf = isec.model->GetMaterial()->EvalBrdf(-ray.direction,isec.nrm,sampledDir);          
            total += illumination.MultiplyPerElement(brdf) * misW ;
          }      
        }
      }
      else
      {
        float pdfLight = _scene->Ambient().GetDirectionalPdf(sampledDir, isec.nrm, isec.worldPosition, isec2.t);
        illumination = _scene->Ambient().Radiance(sampledDir,isec.nrm,0);
        if ( (pdfBrdf > 0) && (pdfLight > 0) )
        {
          float misW = 1.0f / ( pdfBrdf + pdfLight );
          DoAssert(misW > 0);
          DoAssert(pdfBrdf > 0);
          brdf = isec.model->GetMaterial()->EvalBrdf(-ray.direction,isec.nrm,sampledDir);          
          total += illumination.MultiplyPerElement(brdf) * misW ;
        }  
      }
    }

  }
  const int nTechs = 2;
  return total / (nTechs* nSamples);
}

Vector4d PathTraceRenderer::SampleLightBrdf(const Ray & ray, const Intersection & isec)
{
  GCoord = Vector4d(-1,-1,-1,-1);
  // generate the ray according to BRDF
  const Material * m = isec.model->GetMaterial();
  if ( m->IsLight() )
    return m->Emmisive();
  float pdf;
  Vector4d sampledDir ;
  if ( _renderMask & RDirectBRDF )
    sampledDir = m->SampleBrdf( -ray.direction, isec.nrm, pdf );
  if ( _renderMask & RDirectUniform )
    sampledDir = SampleUniform(-ray.direction,isec.nrm,pdf);
#if _DEBUG
  float cc = sampledDir.Dot(isec.nrm);
  //ShowDebugMessage("angle %f\n",cc);
  DoAssert( cc >= 0);
#endif
  if ( sampledDir.Size2() == 0 ) 
    return Vector4d(0,0,0,0); 

  Ray r;
  r.origin = isec.worldPosition;

  // already direction towards outside of the sphere
  r.direction = sampledDir;
  Intersection isec2;
  bool hitSomething = _scene->FindIntersection(r, isec2);

  Vector4d ret(0,0,0,0);
  Vector4d brdf = m->EvalBrdf( -ray.direction, isec.nrm, sampledDir );  
  // if it hits something, better for that to be a light...
  if ( hitSomething )
  {
    bool t = isec2.model->GetMaterial()->IsLight();
    Vector4d illuminationComing = isec2.model->Radiance( isec.nrm, sampledDir, isec2.t );
    DoAssert(t && illuminationComing.Size2()>0)
    ret = illuminationComing.MultiplyPerElement(brdf)/pdf;
  }
  else
  {
    // was not occluded
    // use ambient lighting
    int dummy;
    ret += _scene->Ambient().Radiance(sampledDir,isec.nrm,dummy).MultiplyPerElement(brdf)/pdf;
  }
  return ret;
}
//  
Vector4d PathTraceRenderer::SampleIndirect(const Ray & incomingRay, const Intersection & isec)
{
  Vector4d accum(0,0,0,0);
  Vector4d through(1,1,1,1);
  
  int bounces = _bounces;
  Intersection prev = isec;
  Ray prevRay = incomingRay;
  
  Intersection next;
  float pdf;
  Ray nextRay;
  nextRay.origin = prev.worldPosition;
  // sample the new direction from the previous model ( anywhere on the hemisphere because light can com from everywhere )
  // sample according to brdf
  nextRay.direction = prev.model->GetMaterial()->SampleBrdf(-prevRay.direction,prev.nrm,pdf);
    //frame.InSpace( SampleHemisphereWeighted(0) );

  float cossa = nextRay.direction.Dot(prev.nrm);
  DoAssert(pdf > 0);
  DoAssert(cossa >=0);

  int index = 0;
  // gather the radiance along the path
  while (true)
  {
    index++;
    // if it does not find anything, calculate background light
    // next that will can be possible light
    if ((nextRay.direction.Size2() == 0) || !_scene->FindIntersection(nextRay,next))
    {
      // hack to get full radiance
      accum += through.MultiplyPerElement(_scene->Ambient().Radiance(prev.nrm,prev.nrm,1));
      break;
    }
    
    if ( next.model->GetMaterial()->IsLight())
    {
      accum += through.MultiplyPerElement(next.model->Radiance( prev.nrm, nextRay.direction,next.t ));
    }

    Vector4d brdf = prev.model->GetMaterial()->EvalBrdf(-prevRay.direction, prev.nrm,nextRay.direction);
    float cosa = nextRay.direction.Dot(prev.nrm);
    DoAssert(pdf > 0);
    DoAssert(cosa >=0);
    brdf *= cosa;

    // We generated the direction uniformly
    //float pdf = prev.model->GetMaterial()->GetDirectionalPdf(nextRay.direction,prev.nrm);
    float reflectance = prev.model->GetMaterial()->Reflectance();
    through = through.MultiplyPerElement( brdf ) / (pdf * reflectance);
    if ( _renderMask & (RIndirectSimple |RIndirectNextEvent) )
    {
      // stop according to reflection
      float test = GetFloat();
      if ( test >= reflectance )
        break;
    }
    if ( _renderMask & RIndirectLightBounced )
    {
      bounces--;
      if (bounces == 0 )
        break;
    }

    prev = next;
    prevRay = nextRay;
    nextRay.origin = prev.worldPosition;
    // sample the new direction from the previous model ( anywhere on the hemisphere because light can com from everywhere )
    nextRay.direction = prev.model->GetMaterial()->SampleBrdf(-prevRay.direction,prev.nrm,pdf); 
  }
  //if ( index == 1 )
  //  __debugbreak();
  // nothing, no light
  return accum;
}

void PathTraceRenderer::Init(Scene * scene, Image * image, int maxBounces)
{
  _bounces = maxBounces;
  Renderer::Init(scene, image, maxBounces);
}
