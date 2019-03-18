#include "ToneMapper.h"

int ToneMapper::ToneMap(const float channel)
{
	int out = 0;
	// converts to rgb according to exposure
	float rgb = channel * multiplier;

	// clamping
	if (rgb > 1.0f)
		rgb = 1;

	// display approximation

	// try non linear 
	float p = SRGB_FactorHi * pow(rgb, SRGB_Exponent) - SRGB_Offset;
	rgb = (rgb > SRGB_Cutoff)? p : SRGB_FactorLow * rgb;
	out = rgb * 255;
	return out;
}

void ToneMapper::SetExponent(float exp)
{
	multiplier = pow(2.0f,exp);
}
