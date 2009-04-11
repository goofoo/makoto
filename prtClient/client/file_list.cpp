#include "stdafx.h"
#include "file_list.h"

void file_list::add(const char* fname)
{
// remove existing node
	vector<file_item>::iterator it;
	char* str;
	for(it=container.begin(); it!=container.end(); it++)
	{
		str=(*it).filename();

		if (strcmp(str,fname)==0)
		{
			(*it).setstate(QUEUING);
			bModified=true;
			return;
		}
	}
	
	file_item* p=new file_item(fname);
	p->setstate(QUEUING);
	container.push_back(*p);
	bModified=true;
}

bool file_list::already()// all files have been solved?
{
	vector<file_item>::iterator it;
	for(it=container.begin();it!=container.end();it++)
	{
		if ((*it).getstate()!=DONE)
			return false;
	}
	return true;
}
char* file_list::get()
{
	static char soptfilename[512];
	vector<file_item>::iterator it;
	for (it=container.begin();it!=container.end();it++)
	{
		if ((*it).getstate()!=DONE)
		{
			if ((*it).getstate()==QUEUING ) 
			{
				(*it).setstate(PROCESSING);
				bModified=true;
				sprintf(soptfilename, "%s", (*it).filename());
				return soptfilename;
			}
		}
	}
	return NULL;
}
file_state file_list::getstate(const char* fname)
{
	vector<file_item>::iterator it;
	char* str;
	for(it=container.begin();it!=container.end();it++)
	{
		str=(*it).filename();
		if (strcmp(str,fname)==0)
		{
			return (*it).getstate();
		}
	}
	return QUEUING;
}
bool file_list::setstate(const char* fname, file_state st)
{
	vector<file_item>::iterator it;
	char* str;
	
	for(it=container.begin();it!=container.end();it++)
	{
		str=(*it).filename();
		if (strcmp(str,fname)==0)
		{
			if ((*it).getstate()!=st)
			{
				(*it).setstate(st);
				bModified=true;
			}
			return true;
		}
	}
	return false;
}
char* file_list::list()
{
	vector<file_item>::iterator it;
	static int i=0;
	int j=i;
	bModified=false;
	for(it=container.begin();it!=container.end();it++)
	{
		if((*it).getstate()!=DONE)
		{
			if (j==0)
			{
				i++;
				return (*it).filename();
			}
			if (j>0)
			{
				j--;
			}
		}
	}
	i=0;
	return NULL;
}

bool file_list::IsModified()
{
	return bModified;
}

bool file_list::isQueuing()
{
	vector<file_item>::iterator it;
	for(it=container.begin();it!=container.end();it++)
	{
		if ((*it).getstate() == QUEUING) return true;
	}
	return false;
}

bool file_list::isQueuingOrProcessing()
{
	vector<file_item>::iterator it;
	for(it=container.begin();it!=container.end();it++)
	{
		if ((*it).getstate() == QUEUING || (*it).getstate() == PROCESSING) return true;
	}
	return false;
}

/*Try to end a specific file, only if all tasks are sent*/
void file_list::end(const char* fname)
{
	vector<file_item>::iterator it;
	char* str;
	for(it=container.begin();it!=container.end();it++)
	{
		str=(*it).filename();
		
		if (strcmp(str,fname)==0)
		{
			(*it).setstate(DONE);
			bModified=true;
		}
	}
}
//:~