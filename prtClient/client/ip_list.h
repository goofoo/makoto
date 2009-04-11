#ifndef IP_LIST_H
#define IP_LIST_H
#include <iostream>
#include <vector>
using namespace std;

enum ip_state {BUSY,FREE,NOTCONNECT};

class ip_item
{
	unsigned char ip_array[16];
	ip_state state;
	
public:
	ip_item(const char* ip, ip_state st=NOTCONNECT)
	{
		strcpy((char*)ip_array, ip);
		state=st;
	}
	ip_state getstate()
	{
		return state;
	}
	void setstate(ip_state st)
	{
		state=st;
	}
	char* ipstring()
	{
		return (char*)ip_array;
	}
	void select() {isSelected = 1;}
	void unselect() {isSelected = 0;}
	
	bool isSelected;
};

class ip_list
{
	vector<ip_item> m_ips;
	bool bStateModified; //whether the ips in list state modify?
public:
	ip_list();
	ip_list(const ip_list&);
	void append(const char* ip); //add a ip to the list
	char* get();
	ip_state getstate(const char* ip);
	void setstate(const char* ip, ip_state st);
	char* getfree();
	bool nobusy();
	bool stateModified();
	bool exist(const char* ip);
	
	void selectServer(const char* name);
	void unselectServer(const char* name);
	bool isServerSelected(const char* name);

	int getNumIterm() {return m_ips.size();}
	char* getIterm(int i);
};
#endif //IP_LIST_H