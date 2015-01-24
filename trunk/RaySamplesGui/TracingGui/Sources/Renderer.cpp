#include "renderer.h"
#include "Debug.h"
#include "RandomNumber.h"

void Renderer::Init( Image * image )
{
	_image = image;
}

#if _DEBUG
bool stop = false;
#endif

extern Vector4d GCoord;

Vector4d GMax(0,0,0,0);
Vector4d GMaxCoords;


void Renderer::Render()
{
	_image->Clear();
	GetRendererTrack()->Clear();
  int & iterations = _renderCtx.iterations;
	GetRendererTrack()->SetStages( iterations * _image->W());

	for ( int i =0; i < iterations; i++)
	{
    //Vector4d color = RenderPixel(25,25);
    //_image->AddColor(25,25,color);
    for ( int y = 0; y < _image->H(); y++)
    {
      for ( int x = 0; x < _image->W(); x++)
      {
        if (_stop)
          return;
        Vector4d color = RenderPixel(x,y);
        if ( GCoord [0] > 0)
          _image->AddColor(GCoord[0],GCoord[1],Vector4d(1,1,1,1));
        else
        {
          color[3] = 0;
          if (color[1] > GMax[1])
          {
            GMax = color;
            GMaxCoords[0] = x;
            GMaxCoords[1] = y;
          }
          _image->AddColor(x,y,color);
        }
      }
      GetRendererTrack()->Update();
    }
	}
  GetRendererTrack()->Finish();
}

Renderer::Renderer() : _image(NULL), _stop(false)
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
