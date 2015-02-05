#include "PathTraceRenderer.h"

#include "Debug.h"
#include "RandomNumber.h"

#define MAX_BOUNCES 10
#define  CREATE_MASK(name) | R##name 

static const int RIndirectMask = 0 INDIRECT_LIGHT(CREATE_MASK);

Vector4d PathTraceRenderer::SampleLight(Ray & ray, Intersection & isec)
{
	Vector4d total (0,0,0,0);

	// sample direct light
	for (int iLight =0; iLight< _renderCtx.scene->Lights(); iLight++)
  {
    // sample from light
    Geometry * light = _renderCtx.scene->GetLight(iLight);
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
		bool occluded = _renderCtx.scene->FindIntersection(r2, occSec);

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
	if ( !_renderCtx.scene->FindIntersection(ray,isec) )
	{
		return BLACK;
	}

	if ( _renderCtx.mask &  ( RIndirectMask) )
		total += SampleIndirect(ray,isec);

	if ( _renderCtx.mask & RDirectLight )
		total += SampleLight(ray,isec);
  
  if ( _renderCtx.mask & (RDirectBRDF|RDirectUniform) )
  {
    //isec.nrm = Vector4d(0,0,1,0);
    //isec.worldPosition = Vector4d(0,0,-1.25);
    total += SampleLightBrdf(ray,isec);
  }
  if ( _renderCtx.mask & RDirectMIS )
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


#if _DEBUG
extern bool stop;
#endif

Vector4d PathTraceRenderer::RenderPixel(const int &x, const int &y)
{
#if _DEBUG
  stop = false;
  if ( (x == 23) && (y == 0) )
    stop=true;
#endif
	// we start at camera
  int u = x,v=y;
  if (_renderCtx.mask & COORDS_ONLY)
  {
    u = _renderCtx.fixed[0];
    v = _renderCtx.fixed[1]; 
  }
 
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
    Ray r = _renderCtx.scene->GetRay(401,121);
    _renderCtx.scene->FindIntersection(r,s1);
  }
  else
  {
    v=y;
    Ray r = _renderCtx.scene->GetRay(121,121);
    _renderCtx.scene->FindIntersection(r,s1);
  }
  Ray r = _renderCtx.scene->GetRay(x,v);
  _renderCtx.scene->FindIntersection(r,s2);
  return TrackShine(s2,s1);*/
	Ray ray = _renderCtx.scene->GetRay( u,v );
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
    for (int i =0; i < _renderCtx.scene->Lights(); i++)
    {
      float len;
      illumination = _renderCtx.scene->GetLight(i)->SampleIllumination(isec,sampledDir,len);
      // check for occlusion
      Ray r2;
      r2.origin = isec.worldPosition;
      r2.direction = sampledDir;
      Intersection isec2;
      if ( !_renderCtx.scene->FindIntersection(r2,isec2) || fabs(isec2.t - len) < EPSILON )
      {
        float pdf1 = _renderCtx.scene->GetLight(i)->GetDirectionalPdf(sampledDir, isec.nrm, isec.worldPosition, len);
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
    sampledDir = m->SampleBrdf( -ray.direction, isec.nrm, pdfBrdf,brdf );
    Ray r2;
    r2.origin = isec.worldPosition;
    r2.direction = sampledDir;
    Intersection isec2;
    if ( (sampledDir.Size2() > 0)  )
    {
      if (_renderCtx.scene->FindIntersection(r2,isec2))
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
            //brdf = isec.model->GetMaterial()->EvalBrdf(-ray.direction,isec.nrm,sampledDir);          
            total += illumination.MultiplyPerElement(brdf) * misW ;
          }      
        }
      }
      else
      {
        float pdfLight = _renderCtx.scene->Ambient().GetDirectionalPdf(sampledDir, isec.nrm, isec.worldPosition, isec2.t);
        illumination = _renderCtx.scene->Ambient().Radiance(sampledDir,isec.nrm,0);
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
  Vector4d brdf;
  // generate the ray according to BRDF
  const Material * m = isec.model->GetMaterial();
  if ( m->IsLight() )
    return m->Emmisive();
  float pdf;
  Vector4d sampledDir ;
  if ( _renderCtx.mask & RDirectBRDF )
    sampledDir = m->SampleBrdf( -ray.direction, isec.nrm, pdf,brdf );
  if ( _renderCtx.mask & RDirectUniform )
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
  bool hitSomething = _renderCtx.scene->FindIntersection(r, isec2);

  Vector4d ret(0,0,0,0);
  //Vector4d brdf = m->EvalBrdf( -ray.direction, isec.nrm, sampledDir );  
  // if it hits something, better for that to be a light...
  if ( hitSomething )
  {
    bool t = isec2.model->GetMaterial()->IsLight();
    Vector4d illuminationComing = isec2.model->Radiance( isec.nrm, sampledDir, isec2.t );
    DoAssert(t && illuminationComing.Size2()>0)
    float cosB = isec.nrm.Dot(sampledDir);
    DoAssert(cosB >=0);
    ret = illuminationComing.MultiplyPerElement(brdf)*cosB/(pdf*isec2.t*isec2.t);
  }
  else
  {
    // was not occluded
    // use ambient lighting
    int dummy;
    ret += _renderCtx.scene->Ambient().Radiance(sampledDir,isec.nrm,dummy).MultiplyPerElement(brdf)/pdf;
  }
  return ret;
}

//  
Vector4d PathTraceRenderer::SampleIndirect(const Ray & iRay, const Intersection & firstSec)
{
  Vector4d accum(0,0,0,0);
  Vector4d through(1,1,1,0);
  
  int bounces = _bounces;
  
  Intersection intersection = firstSec;
  
  float pdf;
  
  Ray incomingRay = iRay;
#if _DEBUG
  int index = 0;
#endif
  // gather the radiance along the path
  while (true)
  {
#if _DEBUG
    index++;
#endif
    
    if ( _renderCtx.mask & RIndirectNextEvent )
    {
      // pick up one light and sample from that one light
      // hack - multiply to 0.99 to avoid 1
      int ilight = GetFloat() * 0.9999f * _renderCtx.scene->Lights();
      Geometry * light = _renderCtx.scene->GetLight(ilight);
      Vector4d sample;
      float len;
      Vector4d illuminationA = light->SampleIllumination( intersection, sample, len );
      float pdf = light->GetDirectionalPdf(sample,intersection.nrm,intersection.worldPosition,len);
      // if this light is not occluded
      accum += through.MultiplyPerElement(illuminationA)/(pdf* _renderCtx.scene->TotalArea() );

    }

    if ( intersection.model->GetMaterial()->IsLight())
    {
      // TODO this should not be emmisive...
      accum += through.MultiplyPerElement(intersection.model->GetMaterial()->Emmisive());
    }

    float reflectance = intersection.model->GetMaterial()->Reflectance();
    DoAssert(reflectance <=1.01f);
    if ( _renderCtx.mask & (RIndirectSimple |RIndirectNextEvent) )
    {
      // stop according to reflection
      float test = GetFloat();
      if ( test >= reflectance )
        break;
    }
    if ( _renderCtx.mask & RIndirectLightBounced )
    {
      bounces--;
      if (bounces == 0 )
        break;
    }

    ////////////// NEXT STEP ////////////////////
    Ray bouncedRay;
    bouncedRay.origin = intersection.worldPosition;
    Vector4d brdf;
    bouncedRay.direction = intersection.model->GetMaterial()->SampleBrdf(-incomingRay.direction,intersection.nrm,pdf,brdf);
    // = intersection.model->GetMaterial()->EvalBrdf(-incomingRay.direction, intersection.nrm,bouncedRay.direction);
    float cosa = bouncedRay.direction.Dot(intersection.nrm);
    
    DoAssert(cosa >= 0);
 
    float ff = cosa / (pdf * reflectance);
    Vector4d test = brdf*ff;

    through = through.MultiplyPerElement( brdf ) * cosa / (pdf * reflectance);
    incomingRay = bouncedRay;
    //DoAssert(through.Max() < 1.2);

    if ( (incomingRay.direction.Size2() == 0) || !_renderCtx.scene->FindIntersection(incomingRay,intersection))
    {
      // hack to get full radiance
      Vector4d dummy(1,0,0,0);
      accum += through.MultiplyPerElement(_renderCtx.scene->Ambient().Radiance(dummy,dummy,1));
      break;
    }
    DoAssert(pdf > 0);
    DoAssert(cosa >=0);
  }
  //if ( index == 1 )
  //  __debugbreak();
  // nothing, no light
  return accum;
}

void PathTraceRenderer::Init(Scene * scene, Image * image, int maxBounces)
{
  _bounces = maxBounces;
  Renderer::Init(image);
}
