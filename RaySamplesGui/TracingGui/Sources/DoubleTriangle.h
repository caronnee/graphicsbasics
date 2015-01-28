#ifndef ___DOUBLE_TRIANGLE__
#define ___DOUBLE_TRIANGLE__

#include "Geometry.h"

class DoubleTriangle :  public Geometry
{
  // face points
	Vector4d _points[3];

  // face edges
	Vector4d _edges[2];

  // normal of the triangle
	Vector4d _normal;

  // area of the light
	float _area;

public:
	DoubleTriangle( Vector4d * trianglePoints );
	bool Intersect( const Ray & ray, Intersection & sect );
	void SetPoints(Vector4d * trianglePoints);
	void SetProperty(PropertyType type, void * values);
	void * GetProperty(PropertyType type);
	virtual Vector4d SampleIllumination(const Intersection &section, Vector4d & sampledDir, float & sampleLen);
	void SaveProperties(FileHandler & handler);
	void LoadProperties(FileHandler & handler);
	int Type() const;
  float Area() const;
  Vector4d Radiance( const Vector4d& secNormal, const Vector4d & sampledDir, const float & len );
  virtual float GetDirectionalPdf(const Vector4d & direction, const Vector4d& normal, const Vector4d& pos, const float & len);
};

#endif
