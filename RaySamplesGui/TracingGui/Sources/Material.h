#ifndef __MATERIAL__
#define __MATERIAL__

#include "Vector.h"

class Material
{
	Vector4d _emmisive;
public:
	Material(Vector4d e);
	bool IsLight() const;
	Vector4d Emmisive() const;
	// calculate brdf according to selected material
	virtual Vector4d EvalBrdf(const Vector4d & input, Vector4d & output, float & pdf ) const =0;
};

enum MaterialType{
	MDiffuse
};

Material * CreateMaterial(MaterialType type, const Vector4d & parameters, const Vector4d & emmisive = Vector4d(0,0,0,0));

#endif