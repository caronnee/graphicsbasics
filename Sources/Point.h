#ifndef __POINT__
#define __POINT__

#include "Geometry.h"

class PointObject : public Geometry
{
public:
  void GenerateSurfels(std::vector<Surfel> & surfels, const int & grain);

	virtual bool Intersect( const Ray & ray, Intersection & sect );
	int Type()const;
	Vector4d SampleIllumination(const Intersection &section, Vector4d & sampledDir, float & sampleLen );
  virtual float GetDirectionalPdf(const Vector4d & direction, const Vector4d& normal, const Vector4d& pos, const float & len);
  virtual float Area() const { return 0; }
};
#endif