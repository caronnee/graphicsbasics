#ifndef __CAMERA__
#define __CAMERA__

#include "Matrix.h"
#include "Vector.h"
#include "Point.h"

float toRadians(float angle);

float toDegrees(float angle);

#include <functional>

class Camera : public PointObject
{
	Matrix4d _worldToRaster;
	Matrix4d _rasterToWorld;
	std::function<Vector4d (float&, float &y)> _jittering;
  double _zProj;
public:
	Camera();
	void SetResolution( float x, float y);
	Ray GetRay(float x, float y);
	void SetPerspective(float zFar, float fov);
	Vector4d WorldToRaster(const Vector4d& v) const;
	Vector4d RasterToWorld(const Vector4d& v) const;
	virtual int Type()const;
	virtual Vector4d SampleIllumination(const Intersection &section, Vector4d & sampledDir, float & sampleLen );
  Vector4d WorldToViewport(const Vector4d & mPoint);
};

Camera * GetCamera();

#endif