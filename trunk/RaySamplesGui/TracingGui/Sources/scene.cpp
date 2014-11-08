#include "scene.h"
#include "Plane.h"
#include "FileHandler.h"

AmbientLight& Scene::Ambient()
{
	return _ambientLight;
}

Scene::Scene() :_ambientLight(NULL)
{

}

#if _DEBUG
extern bool stop;
#endif

bool Scene::FindIntersection(const Ray & ray, Intersection& res)
{
	bool found = false;
	res.t = 1e31f;

	// TODO optimize
	for (size_t i =0; i<_geometry.size(); i++)
	{
		Intersection isect;
		Geometry & geom = * _geometry[i];
		
		if ( geom.Intersect(ray,isect) )
		{
			if ( res.t < 0.001)
				continue;
			if ( isect.t < res.t ) // the nearest the best
				res = isect;
			found = true;
		}
	}
	return found;
}

#include "AmbientLight.h"

void Scene::AddModel(Geometry * model)
{
	_geometry.push_back(model);
	if (model->GetMaterial()->IsLight())
	{
		_emmiters.push_back(model);
	}
}

void Scene::Clear()
{
	_ambientLight.SetPower(Vector4d(0,0,0,0));
	_emmiters.clear();
	_emmiters.push_back(&_ambientLight);
	_geometry.clear();
}

int Scene::Lights()
{
	return _emmiters.size();
}

Geometry * Scene::GetLight(int iLight)
{
	return _emmiters[iLight];
}
