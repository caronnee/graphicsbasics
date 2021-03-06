#include "image.h"
#include "Debug.h"
#include <stdio.h>
#include <math.h>
#include <intrin.h>

Image::Image()
{
	_buffer = NULL;
	_w = 0;
	_h = 0;
}

Image::~Image()
{
	delete [] _buffer;
}

void Image::AddColor(int x, int y, const Vector4d & color)
{
  if ( (x >= _w) || (y >= _h) || (x < 0) || (y < 0)  )
    return;
	int index = y*_w + x;
	_buffer[index].Add(color);

}

#include "FileHandler.h"
#include <string>

void Image::SaveHdr(const char * name)
{
	FileHandler file;
	file.Open(name,"wb");
	
	const char * frm = "#?RADIANCE\n# RenderMachine\nFORMAT=32-bit_rle_rgbe\n\n-Y %d +X %d\n\0";
	size_t size = strlen(frm)*2;
	char * c = new char[size];
	sprintf_s(c,size,frm,_w,_h);
	file.Write(c,1,strlen(c));
	
	for ( int i = 0;  i<_w*_h;i++)
	{
		Vector4d& rgbF = _buffer[i];
		float v = rgbF.Max();

		typedef unsigned char byte;
		byte rgbe[4] = {0,0,0,0};

		if(v >= 1e-32f)
		{
			int e;
			v = float(frexp(v, &e) * 1.f / v);
			float test = rgbF.X() * v;
			v*=256;
			rgbe[0] = byte(rgbF.X() * v);
			rgbe[1] = byte(rgbF.Y() * v);
			rgbe[2] = byte(rgbF.Z() * v);
			rgbe[3] = byte(e + 128);
		}

		file.Write((char*)&rgbe[0],1,4);
	}
}

const HDRComponent& Image::GetComponent(int i, int j)const
{
  DoAssert( i < _w);
  DoAssert( j < _h);
  DoAssert( i >= 0);
  DoAssert( j >= 0);
  return _buffer[ j*_w + i];
}

HDRComponent& Image::GetComponent(int i, int j)
{
  DoAssert( i < _w);
  DoAssert( j < _h);
  DoAssert( i >= 0);
  DoAssert( j >= 0);
	return _buffer[ j*_w + i];
}

void Image::Clear()
{
	memset(_buffer,0,_w*_h*sizeof(_buffer[0]));
}

void Image::SetSize(int w, int h)
{
	_w = w;
	_h = h;
	int size = _w*_h;
	if (_buffer)
		delete[] _buffer;
	_buffer = new HDRComponent[size];
	memset(_buffer,0,sizeof(Vector4d)*size);	
}

void Image::Divide(double inv)
{
	for (int i =0; i <_w*_h; i++)
	{
		_buffer[i] *=inv;
	}
}

Image& Image::operator=(const Image& image)
{
  SetSize(image.W(), image.H());
  for ( int i =0; i < image.W(); i++)
    for ( int j =0; j < image.H(); j++ )
      GetComponent(i,j) = image.GetComponent(i,j);
  return *this;
}

