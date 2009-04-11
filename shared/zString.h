#include <stdio.h>
#include <stdlib.h>
#include <string>

inline void windowspath(std::string& in)
{
	int found = in.find(':',0);
	if(found>-1) return;

	int irep = 2;
	while(irep>-1)
	{
		irep = in.find("//",irep);
    		if(irep>=0) in.erase(irep,1);
    	}
    		
	irep = 0;
	while(irep>-1)
	{
		in[irep]='\\';
		irep = in.find('/',irep);
	}
}

inline void sharedpath(std::string& in, int pos, const char* hostname)
{
	int found = in.find(':',0);
	if(found>-1)
	{
		in.erase(0,pos);
		in.insert(0, hostname);
    		in.insert(0, 2, '\\');
    	}
    	
    	windowspath(in);
}

inline void changeFileNameExtension(std::string& name, const char* new_extension)
{
	int found = name.find_last_of('.', name.size()-1);
	if(found < 0) return;
		
	name.erase(found);
	name.append(new_extension);
}

inline void changeFileNameFrame(std::string& name, const int frame, const char* extension)
{
	int found = name.find_first_of('.', 0);
	if(found < 0) return;
		
	name.erase(found);
	
	char buf[5];
	sprintf(buf, ".%d", frame);
	name.append(buf);
	name.append(extension);
}
