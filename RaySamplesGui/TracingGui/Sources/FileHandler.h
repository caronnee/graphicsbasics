#ifndef __HANDLER__
#define __HANDLER__

#include <stdio.h>
#include "String.h"

class FileHandler
{
	FILE * _file;
public:
	FileHandler(); 
	bool Open(const char * name, const char * mode);
	float GetFloat();
	String GetWord();
	bool End()const;
	~FileHandler();
	void Write(void * c, int elementSize, size_t elements);
	void Write( float number );
	void Write( int number );
	void SkipComments();
	void ReadLine();
	void Read(void * value, int len, int count);
	int GetInt();
};

#endif