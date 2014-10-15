
#include "String.h"
#include <string.h>

String::String() :_s(NULL),_len(0),_allocated(0)
{

}
String::String(const char * c, int len)
{
	Create(len);
	memcpy(_s,c,len);
	_len = len;
}

String::String(const char * n)
{
	int len = strlen(n) + 1;
	Create(len);
	memcpy(_s,n,len);
}

String::String(const String& str)
{
	Create(str._allocated);
	Add(str._s, str._len);
}

String& String::operator=(const String & str )
{
	Resize(str._len);
	memcpy(_s,str._s,str._len);
	_len = str._len;
	return *this;
}

void String::Destroy()
{
	if ( _s )
	{
		delete [] _s;
		_s = NULL;
		_len =0;
		_allocated = 0;
	}
}

void String::Create(int size)
{
	_len = 0;
	_allocated = size;
	_s = new char[_allocated];
	memset(_s,0,_allocated);
}

void String::Resize(const int & sizeWanted)
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

void String::Add(char * c, int len)
{
	int sizeWanted = _len +len +1;
	Resize( sizeWanted );
	memcpy( &_s[_len],c,len );
	_len += len;
}

void String::Add(char c)
{
	Add(&c,1);
}

void String::Add(const String & str)
{
	Resize(_len + str._len);
	strcat_s(_s, _allocated, str._s);
	_len = strlen(_s);
}

size_t String::Length()
{
	return _len;
}

const char * String::Str() const
{
	return _s;
}

bool String::EqualI(const String& param)
{
	return _strcmpi(_s,param._s) == 0;
}
