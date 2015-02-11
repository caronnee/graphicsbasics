#ifndef __MATHUTILS__
#define __MATHUTILS__

#define PI 3.14159265359f

#include "Vector.h"

float toRadians(float angle);
float toDegrees(float angle);
Vector4d Reflected( const Vector4d & dir, const Vector4d& normal);
Vector4d GetSpherePosition(float elevation, float azimuth);

#endif