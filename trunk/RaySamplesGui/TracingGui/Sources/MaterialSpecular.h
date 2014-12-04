#ifndef __MATERIAL_SPECULAR__
#define __MATERIAL_SPECULAR__

#include "MaterialDiffuse.h"

class MaterialSpecular : public MaterialDiffuse
{
	Vector4d _specularReflectance;

	float _phongCoef;
public:	
  MaterialSpecular(const Vector4d & diffuse, const Vector4d & specular, const float & phongCoef);

  // 
	Vector4d GetSpecular(const Vector4d & incoming, const Vector4d & normal, const Vector4d & output) const;
  
  //
  Vector4d SampleBrdf(const Vector4d & input,const Vector4d &normal,float &pdf)const;
  
  //
	Vector4d EvalBrdf(const Vector4d & input,const Vector4d & normal, Vector4d & output) const;
};
#endif