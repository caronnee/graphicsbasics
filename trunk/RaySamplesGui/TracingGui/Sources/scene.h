#ifndef __SCENE__
#define __SCENE__

#include "Camera.h"
#include "AmbientLight.h"
#include <vector>

class Scene
{
	AmbientLight _ambientLight;
	std::vector<Geometry *> _geometry;
public:
	Scene();
	void AddModel(Geometry * model);
	AmbientLight& Ambient();
	bool FindIntersection(const Ray & ray, Intersection& isect);
	void CreateCornellBox();
	void Clear();
};
#endif
