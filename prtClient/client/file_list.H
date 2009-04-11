#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;
enum file_state {DONE,QUEUING,PROCESSING};

#define NUMOPTS 8
class file_item
{
public:
	file_item(const char* fname, file_state st=QUEUING)
	{
		file_name = string(fname);
		mystate=st;
//		for(int i=0; i<NUMOPTS; i++) opt[i] = 1;
	}
	file_item() {}
	~file_item(){}
	void setstate(file_state st)
	{
		mystate=st;
	}
	file_state getstate()
	{
		return mystate;
	}
	void print()
	{
		cout << file_name;
	}
	char* filename()
	{
		return (char*)file_name.c_str();
	}
	//int opt[NUMOPTS];
private :
	string file_name;
	file_state mystate;
};
class file_list
{
public:
	void add(const char* fname);
	char* get();
	file_state getstate(const char* fname);
	bool setstate(const char* fname, file_state st);
	char* list();
	bool IsModified();
	bool already();
	file_list()
	{
		bModified=false;
	}
	~file_list()
	{
		container.clear();
	}
	
	int getNumIterm() {return container.size();}
	bool isQueuing();
	bool isQueuingOrProcessing();
	void clear() {container.clear();}
	void end(const char* fname);
	
private:
	vector<file_item> container;
	bool bModified;
};
