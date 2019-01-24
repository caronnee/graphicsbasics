#include "Torus.h"

Torus::Torus(float innerRadius, float tubeRadius)
{
  _R = innerRadius;
  _r = tubeRadius;
}

#include "Roots.h"

int Torus::Type()const
{
  return TypeTorus;
}

void * Torus::GetProperty(PropertyType type)
{
  if (type == PTubeRadius)
    return &_r;
  if (type == PRadius)
    return &_R;
  return nullptr;
}

void Torus::SetProperty(PropertyType type, void * value)
{
  if (type == PTubeRadius)
    _r = *(float*)value;
  if (type == PRadius)
    _R = *(float*)value;
}

void Torus::SaveProperties(FileHandler & handler)
{
  handler.Write(_R);
  handler.Write(_r);
}

void Torus::LoadProperties(FileHandler & handler)
{
  _R = handler.GetFloat();
  _r = handler.GetFloat();
}

void Torus::GenerateSurfels(std::vector<Surfel> & surfels, const int & grain)
{
  throw "Not impleented";
}

#include "Debug.h"

bool Torus::Intersect(const Ray & ray, Intersection & sect)
{
  const Vector4d rayOrigin = WorldToModel(ray.origin);
  const Vector4d rayDirect = WorldToModel(ray.direction);

  // explicit equation: (x^2+y^2+z^2)^2-2(R^2+r^2)(x^2+y^2+z^2)+4R^2y^2+(R2-r2)^2
  // normal equation -> [ 
  // (x^2+y^2+z^2)*2x - (R^2+r^2)x
  // (x^2+y^2+z^2)*2y - (R^2+r^2)y +2R^2y
  // (x^2+y^2+z^2)*2z - (R^2+r^2)z
  // ]

  // dot makes w*w
  double r2mix = _R * _R + _r * _r;
  double d2sum = rayDirect.Dot(rayDirect);
  double od = rayDirect.Dot(rayOrigin);
  double o2sum = rayOrigin.Dot(rayOrigin) -1;
  double o2rmix = o2sum - r2mix;

  double c[5];
  c[4] = d2sum * d2sum;
  c[3] = 4*d2sum*od;
  c[2] = 2*d2sum*o2rmix + 4*od*od + 4*_R*_R*rayDirect.Y()*rayDirect.Y();
  c[1] = 4*o2rmix*od + 8*_R*_R*rayOrigin.Y()*rayDirect.Y();
  c[0] = o2rmix*o2rmix - 4*_R*_R*(_r*_r-rayOrigin.Y()*rayOrigin.Y());

  double s[4];
  int num = Equations::SolveQuartic(c, s);
  if (num == 0)
    return false;
  sect.model = this;
  sect.t = s[0];
  for (int i = 1; i < num; i++)
  {
    if (s[i] < sect.t)
      sect.t = s[i];
  }
  //if (sect.t < EPSILON)
  //  return false;
  Vector4d local = rayOrigin + rayDirect*sect.t;
  // verify
  // explicit equation: (x^2+y^2+z^2)^2-2(R^2+r^2)(x^2+y^2+z^2)+4R^2y^2+(R2-r2)^2
#if 0 //_DEBUG
  double xx = local.X();
  double yy = local.Y();
  double zz = local.Z();
  double a2 = xx * xx + yy * yy + zz * zz;
  double R2 = _R * _R;
  double r2 = _r * _r;
  double ret = a2 * a2 - 2 * (R2 + r2)*a2 + 4 * R2*yy*yy + (R2 - r2)*(R2 - r2);
  DoAssert(fabs(ret) < EPSILON);
#endif

  sect.worldPosition = ModelToWorld(local);
  // normal equation -> [ 
  // (x^2+y^2+z^2)*2x - (R^2+r^2)x
  // (x^2+y^2+z^2)*2y - (R^2+r^2)y +2R^2y
  // (x^2+y^2+z^2)*2z - (R^2+r^2)z
  // ]
  /* let paramSquared = this.sweptRadius*this.sweptRadius + this.tubeRadius*this.tubeRadius;

  let x = point.x;
  let y = point.y;
  let z = point.z;
  let sumSquared = x * x + y * y + z * z;

  let tmp = new Vec3D(
    4.0 * x * (sumSquared - paramSquared),
    4.0 * y * (sumSquared - paramSquared + 2.0*this.sweptRadius*this.sweptRadius),
    4.0 * z * (sumSquared - paramSquared));

  */
  double sum2 = local.X()*local.X() + local.Y()*local.Y()+local.Z()*local.Z();
  double temp = sum2 * 2 - r2mix;
  double x = temp * local.X();
  double z = temp * local.Z();
  temp += 2 * _R*_R;
  double y= temp * local.Y();
  sect.nrm = ModelToWorld(Vector4d(x, y, z, 0));
  sect.nrm.Normalize();
  return true;
}

Vector4d Torus::SampleIllumination(const Intersection &section, Vector4d & sampledDir, float & sampleLen)
{
  throw "Not Implemented";
}