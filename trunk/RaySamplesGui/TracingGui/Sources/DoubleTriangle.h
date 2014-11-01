#ifndef ___DOUBLE_TRIANGLE__
#define ___DOUBLE_TRIANGLE__

#include "Geometry.h"

class DoubleTriangle :  public Geometry
{
	Vector4d _points[3];
	Vector4d _edges[2];
public:
	DoubleTriangle( Vector4d * trianglePoints );
	bool Intersect( const Ray & ray, Intersection & sect );
	void SetPoints(Vector4d * trianglePoints);
	void SetProperty(PropertyType type, void * values);
	void * GetProperty(PropertyType type);
	virtual Vector4d SampleIllumination(Intersection &section, Vector4d & sampledDir, float & len);
	void SaveProperties(FileHandler & handler);
	void LoadProperties(FileHandler & handler);
	int Type() const;
};

#endif
