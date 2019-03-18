
#include "GuiString.h"
#include <string.h>

GuiString::GuiString() :_s(NULL),_len(0),_allocated(0)
{

}
GuiString::GuiString(const char * c, int len)
{
	Create(len);
	memcpy(_s,c,len);
	_len = len;
}

GuiString::GuiString(const char * n)
{
	int len = strlen(n) + 1;
	Create(len);
	memcpy(_s,n,len);
}

GuiString::GuiString(const GuiString& str)
{
	Create(str._allocated);
	Add(str._s, str._len);
}

GuiString& GuiString::operator=(const GuiString & str )
{
	Resize(str._len);
	memcpy(_s,str._s,str._len);
	_len = str._len;
	return *this;
}

void GuiString::Destroy()
{
	if ( _s )
	{
		delete [] _s;
		_s = NULL;
		_len =0;
		_allocated = 0;
	}
}

void GuiString::Create(int size)
{
	_len = 0;
	_allocated = size;
	_s = new char[_allocated];
	memset(_s,0,_allocated);
}

void GuiString::Resize(const int & sizeWanted)
{
	if ( sizeWanted >= _allocated )
	{
		_allocated = sizeWanted *2;
		char * ns = new char[_allocated];
		memset(ns,0,_allocated);
		memcpy(ns,_s,_len);
		delete[] _s;
		_s = ns;
	}
}

void GuiString::Add(char * c, int len)
{
	int sizeWanted = _len +len +1;
	Resize( sizeWanted );
	memcpy( &_s[_len],c,len );
	_len += len;
}

void GuiString::Add(char c)
{
	Add(&c,1);
}

void GuiString::Add(const GuiString & str)
{
	Resize(_len + str._len);
	strcat_s(_s, _allocated, str._s);
	_len = strlen(_s);
}

size_t GuiString::Length()
{
	return _len;
}

const char * GuiString::Str() const
{
	return _s;
}

bool GuiString::EqualI(const GuiString& param)
{
	return _strcmpi(_s,param._s) == 0;
}
