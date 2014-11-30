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

	if (cosA<0)
	{
		return Vector4d(0,0,0,0);	
	}
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

Vector4d MaterialSpecular::SampleBrdf(const Vector4d & input,const Vector4d &normal,float &pdf) const
{
  // sample according to specular BRDF
  Vector4d ret = SampleHemisphere(_phongCoef);
  Vector4d reflected = Reflected( input,normal);
  float cosAn = input.Dot(reflected);
  cosAn = pow(cosAn,_phongCoef);
  Matrix4d sample;
  sample.CreateFromZ(normal);
  // TODO why?
  pdf = (_phongCoef + 1)*cosAn/(2*PI);
  if ( ret.Dot(reflected) < 0 )
    return Vector4d(0,0,0,0);
  return ret;
}
