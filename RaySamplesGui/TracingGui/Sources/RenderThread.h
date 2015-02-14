#ifndef __RENDER_THREAD
#define __RENDER_THREAD

#include <QThread>
#include "Scene.h"
#include "renderer.h"

// _renderCtx.scene->CreateCamera(posTranslate, oMatrix.GetRow(2), oMatrix.GetRow(1) ,fov);
class RenderThread : public QThread
{
	Image _image;
	int _start[2];
	int _end[2];
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