#include "Vector.h"
#include "renderer.h"

class PathTraceRenderer : public Renderer
{
public:
	PathTraceRenderer()  : Renderer()
	{

	}
	virtual Vector4d RenderPixel( const int &x, const int &y );
protected:
private:
};