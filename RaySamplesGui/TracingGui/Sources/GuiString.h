#ifndef __GUI_STRING__
#define __GUI_STRING__

class GuiString
{
	char * _s;
	int _len;
	int _allocated;
	void Resize(const int & sizeWanted);
public:
	GuiString();
	GuiString(const GuiString& str);
	GuiString(const char * n);
	GuiString(const char * c, int len);
	GuiString& operator=(const GuiString & str );
	void Destroy();
	void Create( int size );
	void Add(const GuiString & str);
	void Add(char * c, int len);
	void Add(char c);
	const char * Str() const;
	size_t Length();
	bool EqualI(const GuiString& param1);
};
#endif