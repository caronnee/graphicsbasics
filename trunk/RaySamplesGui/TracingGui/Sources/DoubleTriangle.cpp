#include "DoubleTriangle.h"

DoubleTriangle::DoubleTriangle(Vector4d * trianglePoints)
{
  static Vector4d def [] ={ Vector4d(1,0,0,1),Vector4d(0,1,0,1),Vector4d(1,1,0,1)};
  if ( trianglePoints == NULL )
  {
    trianglePoints = def;
  }
  SetPoints(trianglePoints);
}

void DoubleTriangle::SetPoints(Vector4d * trianglePoints)
{
  memcpy(_points, trianglePoints, sizeof(Vector4d) * 3 );
  _edges[0] = _points[1] -  _points[0];
  _edges[1] = _points[2] -  _points[0];
  _normal = _edges[1].Cross(_edges[0]);
  _area = 0.5f*_normal.Size();
  _normal.Normalize();
}
void DoubleTriangle::SetProperty(PropertyType type, void * values)
{
  if (type == PPoints)
  {
    SetPoints((Vector4d*)values);
  }
}

void * DoubleTriangle::GetProperty(PropertyType type)
{
  if (type == PPoints)
  {
    return _points;
  }
  return NULL;
}

bool DoubleTriangle::Intersect(const Ray & ray, Intersection & sect)
{
  //Möller–Trumbore_intersection_algorithm
  Vector4d d = WorldToModel(ray.direction);
  d.Normalize();
  Vector4d o = WorldToModel(ray.origin);
  //Begin calculating determinant - also used to calculate u parameter
  Vector4d P = d.Cross(_edges[1]);

  //if determinant is near zero, ray lies in plane of triangle
  float det = _edges[0].Dot(P);
  //NOT CULLING
  if(det > -EPSILON && det < EPSILON) 
    return false;

  float inv_det = 1.f / det;

  //calculate distance from V1 to ray origin
  Vector4d T =  o - _points[0];

  //Calculate u parameter and test bound
  float u = T.Dot(P) * inv_det;
  //The intersection lies outside of the triangle
  if(u < 0.f || u > 1.f) 
    return false;

  //Prepare to test v parameter
  Vector4d Q = T.Cross(_edges[0]);

  //Calculate V parameter and test bound
  float v = d.Dot(Q) * inv_det;

  //The intersection lies outside of the triangle
  if(v < 0.f || u + v  > 1.f) 
    return false;

  float t = _edges[1].Dot(Q) * inv_det;

  if(t > EPSILON) { //ray intersection
    sect.t = t;
    sect.model = this;
    sect.worldPosition = ray.origin + ray.direction * t;
    sect.nrm = ModelToWorld(_normal);
    sect.nrm.Normalize();
    return true;
  }

  // No hit, no win
  return false;
}

int DoubleTriangle::Type()const
{
  return TypeTriangle;
}

#include "RandomNumber.h"
#include "Debug.h"

float lasta;
float lastb;
float mmm = 100;

Vector4d DoubleTriangle::Radiance( const Vector4d& secNormal, const Vector4d & sampledDir, const float & len )
{
#if _DEBUG
  float size = sampledDir.Size2();
  DoAssert( fabs( size - 1) < EPSILON);
#endif
  float d2 = len * len;
#if ( 0 &&_DEBUG )
  if ( d2 < 0.006f )
    __debugbreak();
#endif
  float cosA = secNormal.Dot(sampledDir);
  float cosB = -_normal.Dot(sampledDir);
  if ( (cosA <= 0) || (cosB <= 0))
    return Vector4d(0,0,0,0);
  Vector4d li = GetMaterial()->Emmisive();
  return li * cosA ;
}

#include "Camera.h"
#include "debug.h"

Vector4d maxShine;
Vector4d maxCoordsLight;
Vector4d minShine(100,100,100,100);
Vector4d minCoordsLight;

Vector4d DoubleTriangle::SampleIllumination(const Intersection &section, Vector4d & sampledDir, float & sampleLen)
{
  // sample point on the triangle
  float a1 = GetFloat();
  float a2 = GetFloat();

  // sampling from rectangle to triangle 
  if ( a2 > (1 - a1) )
  {
    a1 = 1-a1;
    a2 = 1-a2;
  }
  Vector4d point = _edges[0]*a1 + _edges[1]*a2 + _points[0];
  Vector4d mPoint = ModelToWorld(point);
  sampledDir = mPoint - section.worldPosition;
  sampleLen = sampledDir.Size();
  sampledDir.Normalize();
  Vector4d total = Radiance( section.nrm, sampledDir, sampleLen);
  float pdf = GetDirectionalPdf(sampledDir,section.nrm,section.worldPosition,sampleLen);
  if (pdf <=0)
    return Vector4d(0,0,0,0);
  total/=pdf;

  return total;
}

void DoubleTriangle::SaveProperties(FileHandler & handler)
{
  handler.Write(_points,sizeof(Vector4d),3);	
}

void DoubleTriangle::LoadProperties(FileHandler & handler)
{
  handler.Read(_points,sizeof(Vector4d),3);
  SetProperty(PPoints,_points);
}

float DoubleTriangle::GetDirectionalPdf(const Vector4d & direction, const Vector4d& normal, const Vector4d& pos, const float & len)
{
  float areaPdf = 1.0f/_area;
  float d2 = len * len;
  float geomCoef = _normal.Dot(direction)/-d2;
  return areaPdf/geomCoef;
}

float DoubleTriangle::Area() const
{
  return _edges[0].Cross(_edges[1]).Size()/2.f;
}

#include "MathUtil.h"

void DoubleTriangle::GenerateSurfels(std::vector<Surfel> & surfels, const int & grain)
{
  float step0 = 1.0/grain;
  float step1 = 1.0/grain;
  Surfel surf(this);
  surf.color = Vector4d(0,0,0,0);
  surf.normal = _normal;
  Vector4d p1 = _edges[0]*step0;
  Vector4d p2 = _edges[1]*step1;

  // this should be the radius of the disc
  surf.radius = Vector4d( p1.Max(),p2.Max(),0,0).Max()/3;
  surf.area =  PI * surf.radius * surf.radius;
  Vector4d ustep = _edges[0]*step0;
  Vector4d vstep = _edges[1]*step1;
  for ( float a = 0; a<1; a+=step0)
  {
    bool t = false;
    for ( float b = 0; b<(1-a); b+=step1)
    {
      t = true;
      Vector4d pos0 =  _edges[0]*a + _edges[1]*b;
      Vector4d pos1 = pos0+ustep;
      Vector4d pos2 = pos0+vstep;
      // center of mass of triangle - should be sufficient
      surf.position = ModelToWorld( _points[0] + ( pos0 + pos1 + pos2)/3);
      surfels.push_back(surf);
    }
    DoAssert(t);
  }
}
