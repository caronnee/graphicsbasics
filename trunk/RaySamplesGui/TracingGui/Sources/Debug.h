#ifndef __DEBUG__CHECK__
#define __DEBUG__CHECK__

#include <stdarg.h>
#include <Windows.h>
#include <stdio.h>
#include "Vector.h"

Vector4d MoveToCoord( Vector4d origin, Vector4d dir, float dist , int coord);

#if _DEBUG
#define DoAssert(val) if ( (val)==false ) { OutputDebugStringA(#val); __debugbreak(); }
#else
#define DoAssert(val) if ( (val)==false ) { OutputDebugStringA("Error! Asserionfailed: "); OutputDebugStringA(#val); }
#endif

void ShowDebugMessage(char * format, ...);

#endif