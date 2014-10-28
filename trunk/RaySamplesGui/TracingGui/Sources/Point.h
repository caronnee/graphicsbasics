#ifndef __POINT__
#define __POINT__

#include "Geometry.h"

class PointObject : public Geometry
{
public:
	virtual bool Intersect( const Ray & ray, Intersection & sect );
	int Type()const;
	Vector4d SampleIllumination(Intersection &section, Vector4d & sampledDir, float & sampleLen );
};
#endif