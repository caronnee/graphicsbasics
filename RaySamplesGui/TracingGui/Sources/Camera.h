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
public:
	static Camera * CreateCamera(const Vector4d & position, const Vector4d& direction,const Vector4d& up, float fov );
	Camera();
	void SetResolution( float x, float y);
	Ray GetRay(float x, float y);
	void SetPerspective(float zFar, float fov);
	Vector4d WorldToRaster(const Vector4d& v) const;
	Vector4d RasterToWorld(const Vector4d& v) const;
	virtual int Type()const;
};

#endif