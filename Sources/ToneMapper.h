#ifndef __TONE_MAPPER
#define __TONE_MAPPER

#include "HdrComponent.h"

struct RGBTriplet
{
	char color[3];
};

static const float SRGB_Cutoff    =  0.003041229589676f;
static const float SRGB_FactorHi  =  1.055f;
static const float SRGB_Exponent  =  0.4166666667f;
static const float SRGB_Offset    =  0.055f;
static const float SRGB_FactorLow =  12.92f;

class ToneMapper
{
	float multiplier;
public:
	ToneMapper():multiplier(1) {}
	void SetExponent(float exp);
	int ToneMap( const float channel );
};

#endif
