#include "stdafx.h"
#include "ip_list.h"

ip_list::ip_list(){bStateModified=true;}
ip_list::ip_list(const ip_list& old)
{
	bStateModified=old.bStateModified;
	m_ips=old.m_ips;
}
char* ip_list::getfree()
{
	vector<ip_item>::iterator it;
	static int i=0;
	int j=i;
	for(it=m_ips.begin();it!=m_ips.end(); it++)
	{
		if ((*it).getstate()==FREE)
		{
			i++;
			return (*it).ipstring();
		}
	}
	i=0;
	return NULL;
}
void ip_list::append(const char* ip)
{
	ip_item* p=new ip_item(ip);
	m_ips.push_back(*p);
	bStateModified=true;
}
char* ip_list::get()
{
	vector<ip_item>::iterator it;
	static int i=0;
	int j=i;
	bStateModified=false;
	for(it=m_ips.begin();it!=m_ips.end();it++)
	{
		if (j==0)
		{
			i++;
			return (*it).ipstring();
		}
		if (j>0)
		{
			j--;
		}
	}
	i=0;
	return NULL;
}
ip_state ip_list::getstate(const char* ip)
{
	vector<ip_item>::iterator it;
	char* str;
	for(it=m_ips.begin();it!=m_ips.end();it++)
	{
		str=(*it).ipstring();
		if (strcmp(str, ip)==0)
		{
			return (*it).getstate();
		}
	}
	return NOTCONNECT;
}

void ip_list::setstate(const char* ip, ip_state st)
{
	vector<ip_item>::iterator it;
	char* str;
	
	for(it=m_ips.begin();it!=m_ips.end();it++)
	{
		str=(*it).ipstring();
		if (strcmp(str, ip)==0)
		{
			if ((*it).getstate()!=st)
			{
				(*it).setstate(st);
				bStateModified=true;
			}
		}
	}
}

bool ip_list::nobusy()
{
	vector<ip_item>::iterator it;
	for(it=m_ips.begin();it!=m_ips.end();it++)
	{
		if ((*it).getstate()==BUSY)
			return false;
	}
	return true;
}
bool ip_list::stateModified()
{
	return bStateModified;
}
bool ip_list::exist(const char* ip)
{
	vector<ip_item>::iterator it;
	for(it=m_ips.begin();it!=m_ips.end();it++)
	{
		if(strcmp((*it).ipstring(),ip)==0)
			return true;
	}
	return false;
}

void ip_list::selectServer(const char* name)
{
	vector<ip_item>::iterator it;
	char* str;
	
	for(it=m_ips.begin();it!=m_ips.end();it++)
	{
		str=(*it).ipstring();
		if (strcmp(str, name)==0)
		{
			if(!(*it).isSelected)
			{
				(*it).select();
				bStateModified=true;
			}
		}
	}
}

void ip_list::unselectServer(const char* name)
{
	vector<ip_item>::iterator it;
	char* str;
	
	for(it=m_ips.begin();it!=m_ips.end();it++)
	{
		str=(*it).ipstring();
		if (strcmp(str, name)==0)
		{
			if((*it).isSelected)
			{
				(*it).unselect();
				bStateModified=true;
			}
		}
	}
}

bool ip_list::isServerSelected(const char* name)
{
	vector<ip_item>::iterator it;
	char* str;
	
	for(it=m_ips.begin();it!=m_ips.end();it++)
	{
		str=(*it).ipstring();
		if (strcmp(str, name)==0)
		{
			return (*it).isSelected;
		}
	}
	return 0;
}

char* ip_list::getIterm(int i)
{
	return m_ips[i].ipstring();
}