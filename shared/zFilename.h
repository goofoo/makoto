#include <stdio.h>

class zFilename
{
public:
	zFilename(void);
	~zFilename(void);

	void setName(const char* name) {sprintf(m_name,"%s",name);}
	void setFrameRange(int s, int e) {m_start=s; m_end=e;}
	void getNameAtFrame(char* file, int frame);
	void getExtendedNameAtFrame(char* file, char* extension, int frame);

	char m_name[256];
	int m_start, m_end;
};
