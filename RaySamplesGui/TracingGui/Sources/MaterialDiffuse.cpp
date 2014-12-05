#include "MaterialDiffuse.h"
#include "mathutil.h"

Vector4d MaterialDiffuse::GetDiffuse(const Vector4d & input, const Vector4d & output) const
{
	return _diffuseReflectance / PI;
}

MaterialDiffuse::MaterialDiffuse(const Vector4d & diffuseReflectance, const Vector4d & emmisive) : base(emmisive), _diffuseReflectance (diffuseReflectance)
{
}

Vector4d MaterialDiffuse::EvalBrdf(const Vector4d & input,const Vector4d & normal, Vector4d & output) const
{
	return GetDiffuse(input,output);
}

#include "Matrix.h"
#include "RandomNumber.h"
#include "debug.h"

Vector4d MaterialDiffuse::SampleBrdf(const Vector4d & input,const Vector4d &normal,float &pdf) const
{
  Matrix4d d;
  d.CreateFromZ(normal);
  d = d.Invert();
  Vector4d t = SampleHemisphere();
  Vector4d sample = d * t;
  float cosA = normal.Dot(input);
  DoAssert(normal.Dot(sample) > 0);
  pdf = cosA / PI;
  sample.Normalize();
  return sample;
}

Vector4d MaterialDiffuse::Illumination(Vector4d & sampledDir, const Vector4d & normal, const int &len) const
{
  return Emmisive();
}

