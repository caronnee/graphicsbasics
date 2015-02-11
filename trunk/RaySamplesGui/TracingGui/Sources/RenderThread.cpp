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
	_renderer = CreateRenderer(ctx.renderMask);
	_renderer->_renderCtx = ctx;
	memcpy(_start,ctx.start,sizeof(_start));
	memcpy(_end,ctx.end,sizeof(_end));
}

void RenderThread::run()
{
	_image.SetSize( _end[0], _end[1] );
	_renderer->Init(&_image);
  _renderer->Bake();
	_renderer->Render();
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

void RenderThread::Saturate(float & xDim, float & yDim)
{
  if ( xDim < _end[0])
    xDim = _end[0];
  if ( yDim < _end[1])
    yDim = _end[1];
}
