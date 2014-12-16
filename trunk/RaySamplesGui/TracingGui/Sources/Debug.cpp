#include "Debug.h"

void ShowDebugMessage(char * format, ...)
{
  char buffer[256];
  va_list args;
  va_start (args, format);
  vsprintf (buffer,format, args);
  OutputDebugStringA (buffer);
  va_end (args);
}

Vector4d MoveToCoord(Vector4d origin, Vector4d dir, float dist , int coord)
{
  Vector4d vec = origin;
  float coef = (dist - origin[coord]) /dir[coord];
  return origin + dir * coef;
}
