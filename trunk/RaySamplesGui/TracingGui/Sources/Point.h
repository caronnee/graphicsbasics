#ifndef __POINT__
#define __POINT__

#include "Geometry.h"

class PointObject : public Geometry
{
public:
	virtual bool Intersect( const Ray & ray, Intersection & sect );
	int Type()const;
	Vector4d SampleIllumination(const Intersection &section, Vector4d & sampledDir, float & sampleLen );
  virtual float GetDirectionalPdf(const Vector4d & direction, const Vector4d& normal, const Vector4d& pos, const float & len);
  virtual void GenerateSurfels( std::vector<Surfel> & surfels, const float & grain );
};
#endif