#include "TestRenderer.h"

void TestRenderer::Render()
{
	GetRendererTrack()->Clear();
	GetRendererTrack()->SetStages(_image->W());
	_image->Clear();
	for ( int x = 0; x < _image->W(); x++)
	{
		for ( int y = 0; y < _image->H(); y++)
		{
			Vector4d color = RenderPixel(x,y);
			_image->AddColor(x,y,color);
		}
		GetRendererTrack()->Update();
	}
}

void TestRenderer::Init(Scene * scene, Image * image, Camera * camera)
{
	Renderer::Init(scene,image,camera);
	_bounces = 1 + COEF * _scene->Ligths();
}

