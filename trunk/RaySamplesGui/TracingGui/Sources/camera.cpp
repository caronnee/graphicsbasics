#include "Camera.h"

Camera::Camera()
{
}

void Camera::SetResolution(float resx, float resy)
{
	_worldToRaster.GetRow(0) *= resx;
	_worldToRaster.GetRow(1) *= resy;
	_rasterToWorld = _worldToRaster.Invert();
}

Camera * Camera::CreateCamera(const Vector4d & toCenter, const Vector4d& direction,const Vector4d& up, float fov)
{
	Camera * c = new Camera();
	c->Clear();
	c->SetMatrix(direction,up);
	c->Translate(toCenter);
	float zFar = 1000;
	c->SetPerspective(zFar,fov);
	return c;
}


Ray Camera::GetRay(float x, float y)
{
	Ray r;
	r.origin = ModelToWorld( Vector4d(0,0,0,1) );
	// this is in model space
	Vector4d w (x,y,0,1);
	r.direction = RasterToWorld(w) - r.origin; // modelOrigin
	return r;
}

void Camera::SetPerspective(float zFar, float fov)
{
	float zProj = 1.0f/tan(fov/2);
	float zNear = zProj;
	Vector4d v1(zProj/2.0f,0,0,0.5f);
	Vector4d v2(0,zProj/2.0f,0,0.5f);
	float range = zFar - zNear;
	Vector4d v3(0,0,zFar/range, zFar*zNear/-range);
	Vector4d v4(0,0,1,0);

	_worldToRaster.Identity();
	_worldToRaster.GetRow(0) = v1;
	_worldToRaster.GetRow(1) = v2;
	_worldToRaster.GetRow(2) = v3;
	_worldToRaster.GetRow(3) = v4;

	// move to the correct quadrant
	//_worldToRaster.Translate(Vector4d(1,1,0,0));
	//_worldToRaster.Scale(Vector4d(0.5,0.5,1,1));

	_rasterToWorld = _worldToRaster.Invert();
	Matrix4d res = _worldToRaster*_rasterToWorld;
}
#include "debug.h"

Vector4d Camera::WorldToRaster(const Vector4d& v) const
{
	Vector4d t = WorldToModel(v);
	t = _worldToRaster * t;
	DoAssert(fabs(t[3]) > 0);
	t/=t[3];
	return t;
}
#include "debug.h"

Vector4d Camera::RasterToWorld(const Vector4d& v) const
{
	Vector4d t = _rasterToWorld * v;
	DoAssert(fabs(t[3]) > 0);
	t/=t[3];
	t = ModelToWorld(t);
	return t;
}

int Camera::Type() const
{
	return TypeCamera;
}
