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
	_image->Clear();
	GetRendererTrack()->Clear();
	GetRendererTrack()->SetStages(iterations * _image->W());

	for ( int i =0; i < iterations; i++)
	{
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
	if ( iterations == 1 )
		return; // no need to divide
	double inv = 1.0f/iterations;
	_image->Divide(inv);
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
