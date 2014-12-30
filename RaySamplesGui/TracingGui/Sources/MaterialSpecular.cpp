#include "MaterialSpecular.h"
#include "mathutil.h"
#include "Debug.h"

MaterialSpecular::MaterialSpecular(const Vector4d & diffuse, const Vector4d & specular, const float & phongCoef) : MaterialDiffuse(diffuse, Vector4d(0,0,0,0)),
	_specularReflectance(specular),
	_phongCoef(phongCoef)
{

}

Vector4d MaterialSpecular::GetSpecular(const Vector4d & incoming, const Vector4d & normal, const Vector4d & light) const
{
	// total reflection
#if _DEBUG
	float te = normal.Size();
	DoAssert( fabs(te - 1) < EPSILON );
#endif
	Vector4d R = Reflected(light,normal);
	float cosA = R.Dot(incoming);
	// not in the quarter where reflected ray is, thus cannot be evaluated 
  if (cosA<0)
		return Vector4d(0,0,0,0);	
	float cosAn = pow(cosA,_phongCoef);
	Vector4d v = _specularReflectance * (_phongCoef + 2.0f) * cosAn /( 2 * PI);
	return v;
}

Vector4d MaterialSpecular::EvalBrdf(const Vector4d & incoming, const Vector4d & normal, Vector4d & output) const
{
	return GetDiffuse(incoming,output) + GetSpecular(incoming,normal, output);
}

#include "Matrix.h"
#include "RandomNumber.h"

float MaterialSpecular::GetPdf(const Vector4d &input, const Vector4d & normal) const
{
  Vector4d reflected = Reflected( input, normal );
  float cosAn = normal.Dot(reflected);
  DoAssert( (reflected.Size2() - 1) < EPSILON );
  cosAn = pow(cosAn,_phongCoef);
  float pdf = (_phongCoef + 1)*cosAn/(2*PI);
  return pdf;
}

Vector4d MaterialSpecular::SampleBrdf(const Vector4d & input,const Vector4d &normal,float &pdf) const
{
  float pd = _diffuseReflectance.Max();
  float ps = _specularReflectance.Max();
  float sum = pd + ps;
  float test = GetFloat()*sum;
  if ( test < pd )
  {
    Vector4d ret = MaterialDiffuse::SampleBrdf(input,normal,pdf);
    pdf = pdf * pd/sum;
    return ret;
  }

  // sample according to specular BRDF
  Vector4d ret = SampleHemisphereWeighted(_phongCoef);
  Vector4d reflected = Reflected( input, normal );
  float cosAn = normal.Dot(reflected);
  DoAssert( (reflected.Size2() - 1) < EPSILON );
  DoAssert( (ret.Size2() - 1) < EPSILON );
  cosAn = pow(cosAn,_phongCoef);
  pdf = (_phongCoef + 1)*cosAn/(2*PI);

  Matrix4d sample;
  sample.CreateFromZ(reflected);
  ret = sample.InSpace(ret);
  // TODO why,why?
  if ( ret.Dot(reflected) < 0 || (pdf < EPSILON))
    return Vector4d(0,0,0,0);
  pdf = pdf * ps/sum;
  return ret;
}
