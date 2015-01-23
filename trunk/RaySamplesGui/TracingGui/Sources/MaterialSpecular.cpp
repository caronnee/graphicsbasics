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
	Vector4d R = Reflected(incoming,normal);
	float cosA = R.Dot(light);
	// not in the quarter where reflected ray is, thus cannot be evaluated 
  if (cosA<0  || light.Size2() == 0)
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

float MaterialSpecular::GetSpecularDirectionalPdf(const Vector4d &input, const Vector4d & normal) const
{
  //oBrdfValue = mPhongReflectance * (mPhongExponent + 2) / (PI_F * 2) * pow(/*cos_h*/cosTheta, mPhongExponent);

  float cosAn = input.Dot(normal);
  DoAssert( (normal.Size2() - 1) < EPSILON );
  cosAn = pow(cosAn,_phongCoef);
  float pdfSpec = (_phongCoef + 1)*cosAn/(2*PI);
  return pdfSpec;
}

float MaterialSpecular::GetDirectionalPdf(const Vector4d &input, const Vector4d & normal) const
{
  float pdfDiff = MaterialDiffuse::GetDirectionalPdf(input,normal);
  float pdfSpec = GetSpecularDirectionalPdf(input, normal);

  // normalize
  float pd = _diffuseReflectance.Max();
  float ps = _specularReflectance.Max();
  float sum = ps + pd;

  float ret = (pdfSpec*ps + pd * pdfDiff)/sum;
  DoAssert(ret > 0.1);
  return ret;
}

bool ddd;
Vector4d MaterialSpecular::SampleBrdf(const Vector4d & input,const Vector4d &normal,float &pdf, Vector4d & brdf) const
{
  float pd = _diffuseReflectance.Max();
  float ps = _specularReflectance.Max();
  float sum = pd + ps;
  float test = GetFloat()*sum;
  if ( test < pd )
  {
    ddd = true;
    Vector4d ret = MaterialDiffuse::SampleBrdf(input,normal,pdf, brdf);
    pdf = MaterialDiffuse::GetDirectionalPdf(ret, normal);
    //brdf = MaterialDiffuse::EvalBrdf(ret,normal);
    pdf*=pd/sum;
    return ret;
  }
  ddd = false;

  // sample according to specular BRDF
  Vector4d retSampled = SampleHemisphereWeighted(_phongCoef);
  Vector4d reflected = Reflected( input, normal );

#if _DEBUG
  float cosAn = normal.Dot(reflected);
  DoAssert( (reflected.Size2() - 1) < EPSILON );
  DoAssert( (retSampled.Size2() - 1) < EPSILON );
#endif
  
  Matrix4d normalSpace;
  normalSpace.CreateFromZ(normal);
  Vector4d refLocal = normalSpace.ToLocal(reflected);
  Matrix4d sample;
  sample.CreateFromZ(refLocal); // because we want the sampled ray to be around reflected ray
  Vector4d retRefLocal = sample.InSpace(retSampled);
  Vector4d ret = normalSpace.InSpace(retRefLocal);

  pdf = GetSpecularDirectionalPdf(ret, reflected);  
  pdf *= ps/sum;

  // TODO why,why?
  float aroundN = ret.Dot(normal);
  float aroundR = ret.Dot(reflected);
  if ( (pdf < EPSILON) || (aroundN < 0))
  {
    pdf = 1;
    return Vector4d(0,0,0,0);
  }

  brdf = GetSpecular(input,normal,ret);
  DoAssert(aroundN >= 0);
  return ret;
}

float MaterialSpecular::Reflectance() const
{
  return _specularReflectance.Max()+ MaterialDiffuse::Reflectance();
}
