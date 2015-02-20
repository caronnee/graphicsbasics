#ifndef __IMAGE_HANDLER__
#define __IMAGE_HANDLER__

#include "Vector.h"
#include "HdrComponent.h"

class Image
{
	int _w,_h;
	HDRComponent * _buffer;
public:
	Image();
	~Image();
	void AddColor(int x, int y, const Vector4d & color);
	void SaveHdr(const char * name);
	int W() const { return _w;}
	int H() const { return _h;}
	HDRComponent& GetComponent(int i, int j);
  const HDRComponent& GetComponent(int i, int j)const;
  void Clear();
	void SetSize(int w, int h);
	void Divide(double inv);
  Image& operator=(const Image& image);
};
#endif