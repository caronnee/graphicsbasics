#include "scene.h"
#include "PointLight.h"
#include "Plane.h"
#include "FileHandler.h"

int Scene::Ligths()
{
	return _emitters.size();
}

void Scene::SetAmbientLight(Light * light)
{
	_ambientLight = light;
}

float Scene::Ambient()
{
	if (_ambientLight)
	{
		Vector4d temp(0,0,0,0);
		return _ambientLight->Irradiance(temp,temp);
	}
	return 0;
}

Scene::Scene() :_ambientLight(NULL)
{

}

extern bool stop;

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
}

void Scene::AddLight(Light * light)
{
	_emitters.push_back(light);
	AddModel(light->GetGeometry());
}

void Scene::Clear()
{
	if (_ambientLight )
		_ambientLight->SetPower(0);
	_emitters.clear();
	_geometry.clear();
}
