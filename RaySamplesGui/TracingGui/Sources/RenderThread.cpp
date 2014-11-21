#include "RenderThread.h"

Renderer * CreateRenderer(int t)
{
	switch (t)
	{
	case 0:
		return new TestRenderer();
	case 1:
		return new PathTraceRenderer();
	}
	throw "Rendered not implemented";
}

RenderThread::RenderThread(RenderContext & ctx)
{
	_scene = ctx.scene;
	_renderer = CreateRenderer(ctx.renderMask);
	_renderer->_renderMask = ctx.mask;
	_iterations = ctx.iterations;
	_maxBounces = ctx.bounces;
	memcpy(_start,ctx.start,sizeof(_start));
	memcpy(_end,ctx.end,sizeof(_end));
}

void RenderThread::run()
{
	_image.SetSize( _end[0], _end[1] );
	_renderer->Init(_scene, &_image, _maxBounces);
	_renderer->Render(_iterations);
}

void RenderThread::GetResults(Image & image)
{
	for ( int iDestx=_start[0]; iDestx< _end[0]; iDestx++ )
		for ( int iDesty=_start[1]; iDesty< _end[1]; iDesty++ )
		{
			image.GetComponent(iDestx,iDesty) = _image.GetComponent(iDestx,iDesty);
		}
}

void RenderThread::Stop()
{
	_renderer->Stop();
}
