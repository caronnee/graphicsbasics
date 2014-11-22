#include "renderer.h"
#include "Debug.h"
#include "RandomNumber.h"

void Renderer::Init( Scene * scene, Image * image, int maxBounces )
{
	_image = image;
	_scene = scene;
}

#if _DEBUG
bool stop = false;
#endif

void Renderer::Render(int iterations)
{
	_image->Clear();
	GetRendererTrack()->Clear();
	GetRendererTrack()->SetStages(iterations * _image->W());

	for ( int i =0; i < iterations; i++)
	{
		for ( int x = 0; x < _image->W(); x++)
		{
			for ( int y = 0; y < _image->H(); y++)
			{
				if (_stop)
					return;
				Vector4d color = RenderPixel(x,y);
				_image->AddColor(x,y,color);
			}
			GetRendererTrack()->Update();
		}
	}
	if ( iterations == 1 )
		return; // no need to divide
	double inv = 1.0f/iterations;
	_image->Divide(inv);
}

Renderer::Renderer() : _scene(NULL), _image(NULL), _renderMask(0),_stop(false)
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

void Renderer::Stop()
{
	_stop = true;
}

TrackProgress GTrackProgress;

TrackProgress * GetRendererTrack()
{
	return &GTrackProgress;
}

FiniteBouncer::FiniteBouncer(int bounces)
{
	_maxBounces = bounces;
}

void FiniteBouncer::Init()
{
	_counter = _maxBounces;
}

bool FiniteBouncer::Stop(Ray & ray, Intersection & section, Vector4d & throughput)
{
  if( section.model->GetMaterial()->IsLight())
    return true;
	_counter--;
	throughput *= 1.0f / (_maxBounces +1);
	return  _counter < 0;
}

Ray FiniteBouncer::Bounce(Ray & ray, Intersection & section)
{
	Ray ret;
	ret.origin = section.worldPosition;
	Matrix4d cvrt;
	cvrt.CreateFromZ(section.nrm);
	Vector4d direction = SampleHemisphere();
	ret.direction = cvrt * direction;
	ret.direction.Normalize();
	return ret;
}
