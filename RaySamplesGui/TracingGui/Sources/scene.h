#ifndef __SCENE__
#define __SCENE__

#include "Camera.h"
#include "Light.h"
#include <vector>

class Scene
{
	Light * _ambientLight;
	std::vector<Light *> _emitters;
	std::vector<Geometry *> _geometry;
public:
	Scene();
	void AddModel(Geometry * model);
	int Ligths();
	void SetAmbientLight(Light * l);;
	float Ambient();
	bool FindIntersection(const Ray & ray, Intersection& isect);
	void AddLight(Light * light);
	void CreateCornellBox();
	void Clear();
};
#endif
