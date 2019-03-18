#ifndef __DIRECT_RENDERER__
#define __DIRECT_RENDERER__

#include "Vector.h"
#include "Geometry.h"
#include "scene.h"

// abstract class - does not implement renderer at all
class DirectMCRenderer
{
protected:
  Scene * _scene;
public:
  DirectMCRenderer(Scene * scene);
  Vector4d SampleLight(Ray & ray, Intersection & isec);
};

#endif