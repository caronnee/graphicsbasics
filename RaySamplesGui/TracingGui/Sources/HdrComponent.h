#ifndef __HDRCOMPONENT__
#define __HDRCOMPONENT__

#include "Vector.h"

class HDRComponent: public Vector4d
{
public:
	HDRComponent();
	HDRComponent( const HDRComponent & comp );
	void Normalize();
};
#endif