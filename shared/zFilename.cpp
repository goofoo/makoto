#include "zfilename.h"

zFilename::zFilename(void)
{
	sprintf(m_name,"untitled");
	m_start=0, m_end=0;
}

zFilename::~zFilename(void)
{
}

void zFilename::getNameAtFrame(char* file, int frame)
{
	if(frame<10) 
		{
			sprintf(file, "%s.000%d.tif", m_name, frame);
		}
		else if(frame<100)  
		{
			sprintf(file, "%s.00%d.tif", m_name, frame);
		}
		else if(frame<1000)  
		{
			sprintf(file, "%s.0%d.tif", m_name, frame);
		}
		else if(frame<10000)  
		{
			sprintf(file, "%s.%d.tif", m_name, frame);
		}
}

void zFilename::getExtendedNameAtFrame(char* file, char* extension, int frame)
{
	if(frame<10) 
	{
		sprintf(file, "%s.%s.000%d.tif", m_name, extension, frame);
	}
	else if(frame<100)  
	{
		sprintf(file, "%s.%s.00%d.tif", m_name, extension, frame);
	}
	else if(frame<1000)  
	{
		sprintf(file, "%s.%s.0%d.tif", m_name, extension, frame);
	}
	else if(frame<10000)  
	{
		sprintf(file, "%s.%s.%d.tif", m_name, extension, frame);
	}
}