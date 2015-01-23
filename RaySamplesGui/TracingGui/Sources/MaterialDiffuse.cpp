#include "MaterialDiffuse.h"
#include "mathutil.h"

Vector4d MaterialDiffuse::GetDiffuse(const Vector4d & input, const Vector4d & output) const
{
	return _diffuseReflectance / PI;
}

MaterialDiffuse::MaterialDiffuse(const Vector4d & diffuseReflectance, const Vector4d & emmisive) : Material(emmisive), _diffuseReflectance (diffuseReflectance)
{
}

Vector4d MaterialDiffuse::EvalBrdf(const Vector4d & input,const Vector4d & normal, Vector4d & output) const
{
	return GetDiffuse(input,output);
}

#include "Matrix.h"
#include "RandomNumber.h"
#include "debug.h"

Vector4d MaterialDiffuse::SampleBrdf(const Vector4d & input,const Vector4d &normal,float &pdf, Vector4d & brdf) const
{
  Matrix4d d;
  d.CreateFromZ(normal);
  Vector4d t = SampleHemisphereWeighted();
  float cosA = t.Z();
  Vector4d sample = d.InSpace(t);
  DoAssert(cosA >= 0);
#if _DEBUG
  float tt  = normal.Dot(sample);
  DoAssert(fabs(cosA - tt) < EPSILON);
#endif
  DoAssert(normal.Dot(sample) > 0);
  pdf = cosA / PI;
  sample.Normalize();
  brdf = GetDiffuse(sample,normal);
  return sample;
}

float MaterialDiffuse::GetDirectionalPdf(const Vector4d &sampledDir, const Vector4d & nrm) const
{
  return sampledDir.Dot(nrm)/PI;
}

float MaterialDiffuse::Reflectance() const
{
  return _diffuseReflectance.Max();
}
