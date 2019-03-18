#include "Material.h"
#include "MathUtil.h"
#include "GuiString.h"
#include "Debug.h"
#include <stdio.h>
#include "RandomNumber.h"
#include "Matrix.h"
#include <list>

std::list<Material *> GMaterialBank;

void ClearMaterials()
{
  for each ( auto m in GMaterialBank)
  {
    delete m;
  }
  GMaterialBank.clear();
}

//
Material * CreateMaterial(const Vector4d diffuse)
{
  Material * m = new Material(diffuse, Vector4d(), 0);
  GMaterialBank.push_back(m);
  return m;
}

Material * CreateLight(const Vector4d emmisive)
{
  Material * m = new Material(emmisive);
  GMaterialBank.push_back(m);
  return m;
}
Material * CreateMaterial(const Vector4d diffuse, const Vector4d spec, float phong)
{
  Material * m = new Material(diffuse, spec, phong);
  GMaterialBank.push_back(m);
  return m;
}
Vector4d Material::GetSpecular(const Vector4d & incoming, const Vector4d & normal, const Vector4d & light) const
{
  // total reflection
#if _DEBUG
  float te = normal.Size();
  DoAssert(fabs(te - 1) < EPSILON);
#endif
  Vector4d R = Reflected(incoming, normal);
  float cosA = R.Dot(light);
  // not in the quarter where reflected ray is, thus cannot be evaluated 
  if (cosA<0 || light.Size2() == 0)
    return Vector4d(0, 0, 0, 0);
  float cosAn = pow(cosA, _phongCoef);
  return _specularReflectance * (_phongCoef + 2.0f) * cosAn / (2 * PI);
}

float Material::GetSpecularDirectionalPdf(const Vector4d &input, const Vector4d & reflected) const
{
  //oBrdfValue = mPhongReflectance * (mPhongExponent + 2) / (PI_F * 2) * pow(/*cos_h*/cosTheta, mPhongExponent);

  float cosAn = input.Dot(reflected);
  DoAssert((reflected.Size2() - 1) < EPSILON);
  cosAn = pow(cosAn, _phongCoef);
  float pdfSpec = (_phongCoef + 1)*cosAn / (2 * PI);
  return pdfSpec;
}

Vector4d Material::EvalBrdf(const Vector4d & incoming, const Vector4d & normal, Vector4d & output) const
{
  return GetDiffuse(incoming, output) + GetSpecular(incoming, normal, output);
}

Vector4d Material::GetDiffuse(const Vector4d & input, const Vector4d & output) const
{
  return _diffuseReflectance / PI;
}

bool Material::IsLight()const
{
  return _emmisive.Size2() > 0;
}

float Material::Reflectance() const
{
  return  _specularReflectance.Max() + _diffuseReflectance.Max();
}

float Material::GetDirectionalPdf(const Vector4d &input, const Vector4d & normal) const
{
  float pdfDiff = input.Dot(normal) / PI;
  if (!_phongCoef)
    return pdfDiff;

  float pdfSpec = GetSpecularDirectionalPdf(input, Reflected(input, normal));

  // normalize
  float pd = _diffuseReflectance.Max();
  float ps = _specularReflectance.Max();
  float sum = ps + pd;

  float ret = (pdfSpec*ps + pd * pdfDiff) / sum;
  DoAssert(ret > 0.1);
  return ret;
}

Material::Material(Vector4d e) :
  _emmisive(e),
  _diffuseReflectance(),
  _specularReflectance(),
  _phongCoef(0)
{
}

Material::Material(Vector4d d, Vector4d s, float phong) :
  _emmisive(),
  _diffuseReflectance(d),
  _specularReflectance(s),
  _phongCoef(phong)
{
  float r = Reflectance();
  if (r > 1)
  {
    _diffuseReflectance /= r;
    _specularReflectance /= r;
  }
}

Vector4d Material::Emmisive() const
{
  return _emmisive;
}

Vector4d Material::SampleBrdf(const Vector4d & input, const Vector4d &normal, float &pdf, Vector4d & brdf) const
{
  Matrix4d d;
  d.CreateFromZ(normal);
  Vector4d t = SampleHemisphereWeighted();
  float cosA = t.Z();
  Vector4d ret = d.InSpace(t);
  DoAssert(cosA >= 0);
#if _DEBUG
  float tt = normal.Dot(ret);
  DoAssert(fabs(cosA - tt) < EPSILON);
#endif
  DoAssert(normal.Dot(ret) > 0);
  pdf = cosA / PI;
  ret.Normalize();
  brdf = GetDiffuse(ret, normal);
  if (!_phongCoef)
    return ret;
  // specular
  float pd = _diffuseReflectance.Max();
  float ps = _specularReflectance.Max();
  float sum = pd + ps;
  float test = GetFloat()*sum;
  if (test < pd)
  {
    pdf *= pd / sum;
    return ret;
  }

  // sample according to specular BRDF
  Vector4d retSampled = SampleHemisphereWeighted(_phongCoef);
  Vector4d reflected = Reflected(input, normal);

  Matrix4d normalSpace;
  normalSpace.CreateFromZ(normal);
  Vector4d refLocal = normalSpace.ToLocal(reflected);
  Matrix4d sample;
  sample.CreateFromZ(refLocal); // because we want the sampled ray to be around reflected ray
  Vector4d retRefLocal = sample.InSpace(retSampled);
  ret = normalSpace.InSpace(retRefLocal);

  pdf = GetSpecularDirectionalPdf(ret, reflected);
  pdf *= ps / sum;

  // TODO why,why?
  float aroundN = ret.Dot(normal);
  float aroundR = ret.Dot(reflected);
  if ((pdf < EPSILON) || (aroundN < 0))
  {
    pdf = 1;
    return Vector4d(0, 0, 0, 0);
  }

  brdf = GetSpecular(input, normal, ret);
  DoAssert(aroundN >= 0);
  return ret;
}
