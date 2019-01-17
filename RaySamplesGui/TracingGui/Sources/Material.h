#ifndef __MATERIAL__
#define __MATERIAL__

#include "Vector.h"

// base of the materials
class Material
{
private:
	Vector4d _emmisive;
  Vector4d _diffuseReflectance;
  Vector4d _specularReflectance;
  float _phongCoef;

public:
  Material(Vector4d e);
  Material(Vector4d d, Vector4d s, float p);
	bool IsLight() const;
	Vector4d Emmisive() const;
	// calculate brdf according to selected material
  Vector4d EvalBrdf(const Vector4d & input, const Vector4d & normal, Vector4d & outputDir) const;
  Vector4d SampleBrdf(const Vector4d & input,const Vector4d &normal, float &pdf, Vector4d &brdf)const;
  float GetDirectionalPdf(const Vector4d &sampledDir, const Vector4d & nrm) const;
  float Reflectance() const;
  ///
  Vector4d GetDiffuse(const Vector4d & input, const Vector4d & output) const;
  Vector4d GetSpecular(const Vector4d & incoming, const Vector4d & normal, const Vector4d & light) const;
  float Material::GetSpecularDirectionalPdf(const Vector4d &input, const Vector4d & reflected) const;
};

Material * CreateMaterial(const Vector4d  diffuse);
Material * CreateLight(const Vector4d emmisive);
Material * CreateMaterial(const Vector4d diffuse, const Vector4d spec, float phong);
void ClearMaterials();

#endif