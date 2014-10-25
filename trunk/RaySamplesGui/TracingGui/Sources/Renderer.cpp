#include "renderer.h"
#include "Debug.h"

void Renderer::Init(Scene * scene, Image * image, Camera * camera)
{
	_image = image;
	_scene = scene;
	_camera = camera;
}

#if _DEBUG
bool stop = false;
#endif

void Renderer::Render(int iterations, int & mask)
{
	GetRendererTrack()->Clear();
	GetRendererTrack()->SetStages(_image->W());
	_image->Clear();
	for ( int x = 0; x < _image->W(); x++)
	{
		for ( int y = 0; y < _image->H(); y++)
		{
			Vector4d color = RenderPixel(x,y,mask);
			_image->AddColor(x,y,color);
		}
		GetRendererTrack()->Update();
	}
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
