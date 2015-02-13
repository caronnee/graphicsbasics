#ifndef __scene__
#define __scene__

#include "Camera.h"
#include "AmbientLight.h"
#include <vector>

struct CameraContext
{
	float resolution[2];
	float fov;
	Vector4d axis[2];
	Vector4d position;
};

class Scene
{
	Camera * _camera;
	AmbientLight _ambientLight;
	std::vector<Geometry *> _geometry;
	std::vector<Geometry *> _emmiters;
  float _totalArea;
	void DeleteModel(Geometry * geometry);
public:
	Scene();

  Camera * GetCamera() { return _camera; }
	Ray GetRay(float x,float y);
	void AddModel(Geometry * model);
	void CreateAddCamera( CameraContext & ctx);
	AmbientLight& Ambient();
	bool FindIntersection(const Ray & ray, Intersection& isect);
	void CreateCornellBox();
	void Clear();
	int Lights();
	Geometry * GetLight(int iLight);
  const float TotalArea() const;
  Geometry * Model(int i) const;
  Camera * SwitchCamera( Camera * camera );
};
#endif
