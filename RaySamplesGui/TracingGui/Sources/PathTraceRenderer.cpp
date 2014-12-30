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
  return lightSection.model->Evaluate( testSection.nrm, dirToLight, len,1);
}

Vector4d PathTraceRenderer::RenderPixel(const int &x, const int &y)
{
	// we start at camera
  int u = x,v=y;
  //u = 89;
  //v = 158;  
  //if ( y < 128 )
  {
   // u = 60;
  }
  //else
  {
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
	if (_bouncer)
		_bouncer->Init();
	Ray ray = _scene->GetRay( u,v );
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

Vector4d GCoord;

#include "MathUtil.h"

Vector4d PathTraceRenderer::SampleMIS(const Ray & ray, const Intersection & isec)
{
  const Material * m = isec.model->GetMaterial();
  if ( m->IsLight() )
    return m->Emmisive();

  Vector4d total;
  // number of mis sampling is 1 for start
  float pdf;
  Vector4d sampledDir;
  Vector4d illumination;
  Vector4d brdf;
  // sample from one light
  {
    int i = GetFloat() * _scene->Lights();
    float len;
    illumination = _scene->GetLight(i)->SampleIllumination(isec,sampledDir,len);
    // check for occlusion
    Ray r2;
    r2.origin = isec.worldPosition;
    r2.direction = sampledDir;
    Intersection isec2;
    if ( !_scene->FindIntersection(r2,isec2) || fabs(isec2.t - len) < EPSILON )
    {
      float pdf1 = _scene->GetLight(i)->GetPdf(sampledDir) / _scene->Lights();
      float pdf2 = isec.model->GetMaterial()->GetPdf(sampledDir,isec.nrm);
      brdf = isec.model->GetMaterial()->EvalBrdf(-ray.direction,isec.nrm,sampledDir);
      if ( (pdf1 > 0) && (pdf2 > 0) )
      {
        float misW = pdf1 + pdf2;
        DoAssert(misW > 0);
        DoAssert(pdf1 > 0);
        total += illumination.MultiplyPerElement(brdf) * misW/pdf1;
      }
    }
  }

  //sample from brdf
  {
    int i = GetFloat() * _scene->Lights();
    float pdf1;
    sampledDir = m->SampleBrdf( -ray.direction, isec.nrm, pdf1 );
    Ray r2;
    r2.origin = isec.worldPosition;
    r2.direction = sampledDir;
    Intersection isec2;
    if ( sampledDir.Size2() > 0 && _scene->FindIntersection(r2,isec2) )
    {
      if (isec2.model->GetMaterial()->IsLight())
      {
        float pdf2 = isec2.model->GetPdf(sampledDir);
        illumination = isec2.model->Evaluate(isec.nrm,sampledDir,isec2.t,pdf1);
        if ( (pdf1 > 0) && (pdf2 > 0) )
        {
          float misW = pdf1 + pdf2;
          DoAssert(misW > 0);
          DoAssert(pdf1 > 0);
          total += illumination.MultiplyPerElement(brdf) * misW /pdf1;
        }      
      }
    }
  }
  
  return total;
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
    Vector4d illuminationComing = isec2.model->Evaluate( isec.nrm, sampledDir, isec2.t,pdf );
    DoAssert(t && illuminationComing.Size2()>0)
    ret = illuminationComing.MultiplyPerElement(brdf);
  }
  else
  {
    // was not occluded
    // use ambient lighting
    int dummy;
    ret += _scene->Ambient().Illumination(sampledDir,isec.nrm,dummy,pdf).MultiplyPerElement(brdf);
  }
  return ret;
}
