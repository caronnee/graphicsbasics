#include "renderer.h"
#include "Debug.h"

void Renderer::Init(Scene * scene, Image * image, Camera * camera)
{
	_image = image;
	_scene = scene;
	_camera = camera;
}

bool stop = false;

Vector4d Renderer::RenderPixel( int x, int y)
{
	if ( (x == 34) && (y == 63) )
		stop = true;
	else
		stop = false;
	Ray ray = _camera->GetRay((float)x +0.5,(float)y +0.5);
	Vector4d total = Vector4d(0,0,0,0);
	// so far only ambient
	Intersection isect;
	bool found = _scene->FindIntersection(ray, isect);
	if ( !found )
	{
//		DoAssert(false);
		return BLACK; //nothing got rendered in this pixel
	}
	{
		if ( stop )
		{
			Vector4d test = Vector4d(0,0,1,0);
			Vector4d test1 = ray.direction;
			test1.Normalize();
			float ac = acos(test.Dot(test1));
			float angle = toDegrees(ac);
		}
		
	}
	//eval brdf
	Vector4d brdf = isect.model->GetMaterial()->GetReflectance( isect.positionModel, -ray.direction);
	total = brdf;
	return total;
}

Renderer::Renderer() :_camera(NULL), _scene(NULL), _image(NULL)
{

}

void Renderer::Destroy()
{
	/* TODO*/
}

Renderer::~Renderer()
{
	Destroy();
}


TrackProgress GTrackProgress;

TrackProgress * GetRendererTrack()
{
	return &GTrackProgress;
}
