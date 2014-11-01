#ifndef __SPHERE_MODEL__
#define __SPHERE_MODEL__

#include "Geometry.h"

class Sphere : public Geometry
{
	typedef Geometry base;
public:
	Sphere( float radius);
	virtual bool Intersect( const Ray & ray, Intersection & sect );
	virtual Vector4d SampleIllumination(Intersection &section, Vector4d & sampledDir, float & len);
	int Type()const;
	void * GetProperty(PropertyType type);
	void SetProperty(PropertyType type, void * value);
	void SaveProperties(FileHandler & handler);
	void LoadProperties(FileHandler & handler);
	float _radius;
	float _radius2;
};

#endif