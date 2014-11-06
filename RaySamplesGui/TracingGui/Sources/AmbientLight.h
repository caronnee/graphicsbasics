#ifndef __AMBIENT_LIGHT__
#define __AMBIENT_LIGHT__

#include "Vector.h"

class AmbientLight
{
	Vector4d _backColor;
public:
	AmbientLight( const Vector4d & watts );
	void SetPower( const Vector4d & watts );
	Vector4d SampleIllumination(const Vector4d & incoming,const Vector4d & norm, Vector4d& outputDirection);
};
#endif