#ifndef __AMBIENT_LIGHT__
#define __AMBIENT_LIGHT__

#include "Vector.h"
#include "Geometry.h"

class AmbientLight : public Geometry
{
	Vector4d _backColor;
public:
	AmbientLight( const Vector4d & watts );
	void SetPower( const Vector4d & watts );
	bool Intersect(const Ray &,Intersection &);
	Vector4d SampleIllumination(const Intersection &section, Vector4d & sampledDir, float & sampleLen );
  
  // Type og the object, created by enum
	int Type(void) const;
  
  // weighted illumination
  Vector4d Illumination(const Vector4d & lightVector, const Vector4d & nrm, const int & dummy, float pdf);

  float GetPdf(const Vector4d & direction);
};
#endif