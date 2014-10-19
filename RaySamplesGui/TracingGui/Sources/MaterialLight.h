#ifndef __LIGHT__
#define __LIGHT__

#include "Vector.h"

class LightMaterial
{
public:
	virtual void SetPower( const Vector4d & watts ) = 0;
	virtual Vector4d Emit ( const Vector4d & inputDirection, const Vector4d& outputDirection ) = 0;
};
#endif