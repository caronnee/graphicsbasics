#include "MathUtil.h"
#include "Debug.h"

float toRadians(float angle)
{
	return angle *PI/180;
}

float toDegrees(float angle)
{
	return angle * 180/PI;
}

Vector4d Reflected(const Vector4d & dir, const Vector4d& normal)
{
  DoAssert( fabs(normal.Size2() -1) < EPSILON );
  DoAssert( fabs(dir.Size2() -1) < EPSILON );
  Vector4d reflected = normal*2*normal.Dot(dir) - dir;
  reflected.Normalize();
  return reflected;
}
