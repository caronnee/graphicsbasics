#include "FileHandler.h"
#include <Windows.h>

FileHandler::FileHandler() : _file(NULL)
{

}

FileHandler::~FileHandler()
{
	if (_file)
	{
		fclose(_file);
		_file = NULL;
	}
}

bool FileHandler::Open(const char * n, const char * mode)
{
	GuiString name(n);
	if ( name.Str()[2] != ':' )
	{
		char c[128];
		GetModuleFileNameA(NULL,c,128);
		const char * end = strrchr(c,'\\');
		int len = end - c +1;
		GuiString str(c,len);
		str.Add(name);
		name = str;
	}
	int err = fopen_s(&_file,name.Str(),mode);
	return err == 0;
}

float FileHandler::GetFloat()
{
	float f = 0;
	fread(&f,sizeof(float),1,_file);
	return f;
}

GuiString FileHandler::GetWord()
{
	GuiString ret;
	while ( !End() )
	{
		char c = getc(_file);
		if ( (c== '\n') || (c==' ') )
		{
			return ret;
		}
		ret.Add(c);
	}
	return ret;
}

bool FileHandler::Valid() const
{
	return _file != NULL;
}
bool FileHandler::End() const
{
	return feof(_file)!=0;
}

void FileHandler::Write(void * c, int elementSize, size_t elements)
{
	fwrite(c, elementSize, elements,_file);
}

void FileHandler::Write(float number)
{
	Write(&number,sizeof(float),1);
}

void FileHandler::Write(int number)
{
	Write(&number, sizeof(int),1);
}

void FileHandler::ReadLine()
{
	while ( !End() && getc(_file)!='\n');
}

void FileHandler::SkipComments()
{
	if ( End() )
		return;
	if ( getc(_file) == '#' )
		ReadLine();
	else
		fseek( _file, -1, SEEK_SET );
}

void FileHandler::Read(void * value, int len, int count)
{
	fread(value,len, count,_file);
}

int FileHandler::GetInt()
{
	int val;
	Read(&val,sizeof(int),1);
	return val;
}

