#ifndef __MATERIAL_DIFUSE__
#define __MATERIAL_DIFUSE__

#include "Material.h"

class MaterialDiffuse : public Material
{
protected:
  typedef Material base;
  Vector4d _diffuseReflectance;
public:

	MaterialDiffuse(const Vector4d & diffuseReflectance,const Vector4d & emmisive);
	Vector4d GetDiffuse( const Vector4d & input, const Vector4d & output ) const;
	Vector4d EvalBrdf(const Vector4d & input,const Vector4d & normal, Vector4d & output ) const;
  Vector4d SampleBrdf(const Vector4d & input,const Vector4d &normal,float &pdf)const;
  float GetDirectionalPdf(const Vector4d &sampledDir, const Vector4d & nrm) const;
  virtual float Reflectance() const;
};

#endif