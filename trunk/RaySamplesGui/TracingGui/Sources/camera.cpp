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

Ray Camera::GetRay(float x, float y)
{
	Ray r;
	r.origin = ModelToWorld( Vector4d(0,0,0,1) );
	// this is in model space
	Vector4d w (x,y,0,1);
  Vector4d tt = RasterToWorld(w);
	r.direction = RasterToWorld(w) - r.origin; // 
  r.direction.Normalize();
  Vector4d test = WorldToRaster( r.direction );
	return r;
}

void Camera::SetPerspective(float zFar, float fov)
{
	_zProj = 1.0f/tan(fov/2);
	float zNear = _zProj;
	Vector4d v1(_zProj/2.0f,0,0,_zProj/2);
	Vector4d v2(0, -_zProj/2.0f,0,_zProj/2);
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
}
#include "debug.h"

Vector4d Camera::WorldToRaster(const Vector4d& v) const
{
	Vector4d t2 = WorldToModel(v);
	Vector4d t = _worldToRaster * t2;
	DoAssert((fabs(t[3]) > 0) || (t2.Size2() == 0));
  if ( t[3] <=0)
    return Vector4d(-1,-1,-1,-1);
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

Vector4d Camera::SampleIllumination(const Intersection &section, Vector4d & sampledDir, float & sampleLen)
{
	Vector4d zero;
	zero.Zero();
	return zero;
}

Vector4d Camera::WorldToViewport(const Vector4d & mPoint)
{
  Vector4d ret = WorldToRaster(mPoint);
  Vector4d pos = Position();
  Vector4d testDir = ret - WorldToRaster(pos);
  testDir *= ret[2] / testDir[2];//normalize Z
  ret -= testDir;
  ret /= ret[3];
#if 0&& _DEBUG // works only when point is not behind the mpoint
  Vector4d th = RasterToWorld(Vector4d(ret[0],ret[1],0,1));
  Vector4d dir0 = mPoint - th;
  dir0.Normalize();
  Vector4d dir1 = mPoint - Position();
  dir1.Normalize();
  float te = dir1.Dot(Direction());
  DoAssert(te > 0);
  te = dir0.Dot(Direction());
  DoAssert(te > 0);
  DoAssert(dir0 == dir1);
#endif
  return ret;
}
