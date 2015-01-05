#ifndef __RENDER_THREAD
#define __RENDER_THREAD

#include <QThread>
#include "Scene.h"
#include "renderer.h"
#include "TestRenderer.h"
#include "PathTraceRenderer.h"

struct RenderContext
{
	// scene contains also camera
	Scene * scene;
	int mask;
	int start[2];
	int end[2];
	int renderMask;
	int iterations;
	int bounces;
};

// _scene->CreateCamera(posTranslate, oMatrix.GetRow(2), oMatrix.GetRow(1) ,fov);
class RenderThread : public QThread
{
	Image _image;
	Scene *_scene;
	int _mask;
	int _start[2];
	int _end[2];
	int _iterations;
	int _maxBounces;

	// manually created:
	Renderer * _renderer;

private:
	void run();

public:	
	RenderThread( RenderContext & ctx);
	void GetResults(Image & image);
	void Stop();
  void Saturate(float & xDim, float & yDim);
};

#endif