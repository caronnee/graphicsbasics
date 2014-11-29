#ifndef __MATERIAL_DIFUSE__
#define __MATERIAL_DIFUSE__

#include "Material.h"

class MaterialDiffuse : public Material
{
  typedef Material base;
  Vector4d _diffuseReflectance;
  Vector4d _emmisive;
public:

	MaterialDiffuse(const Vector4d & diffuseReflectance,const Vector4d & emmisive);
	Vector4d GetDiffuse( const Vector4d & input, const Vector4d & output ) const;
	Vector4d EvalBrdf(const Vector4d & input,const Vector4d & normal, Vector4d & output ) const;
  Vector4d SampleBrdf(const Vector4d & input,const Vector4d &normal,float &pdf)const;
};

#endif