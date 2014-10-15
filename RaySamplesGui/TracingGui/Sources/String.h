#ifndef __STRING__
#define __STRING__

class String
{
	char * _s;
	int _len;
	int _allocated;
	void Resize(const int & sizeWanted);
public:
	String();
	String(const String& str);
	String(const char * n);
	String(const char * c, int len);
	String& operator=(const String & str );
	void Destroy();
	void Create( int size );
	void Add(const String & str);
	void Add(char * c, int len);
	void Add(char c);
	const char * Str() const;
	size_t Length();
	bool EqualI(const String& param1);
};
#endif