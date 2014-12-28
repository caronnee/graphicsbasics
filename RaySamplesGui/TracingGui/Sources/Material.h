#ifndef __MATERIAL__
#define __MATERIAL__

#include "Vector.h"

// base of the materials
class Material
{
	Vector4d _emmisive;
public:
	Material(Vector4d e);
	bool IsLight() const;
	Vector4d Emmisive() const;
	// calculate brdf according to selected material
	virtual Vector4d EvalBrdf(const Vector4d & input, const Vector4d & normal, Vector4d & outputDir ) const =0;
  virtual Vector4d SampleBrdf(const Vector4d & input,const Vector4d &normal, float &pdf)const = 0;
};

enum MaterialType{
	MDiffuse,
  MSpecular
};

Material * CreateMaterial(int type, const Vector4d * parameters, float phong);
#endif