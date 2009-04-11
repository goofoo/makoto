#include "refresh_iplist.h"
#include "ip_list.h"
#include <winsock2.h>
#include <string>
#include <iostream>
using namespace std;
ip_list* g_iplist;

ip_state isFree(const char* ip)
{
	SOCKET s=socket(AF_INET,SOCK_STREAM,0);
	sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(2000);
	addr.sin_addr.S_un.S_addr=inet_addr(ip);
	if (connect(s, (SOCKADDR*)&addr,sizeof(addr))!=0)
	{
		closesocket(s);
		return NOTCONNECT;
	}
	send(s, "host free",10,0);
	char rbuf[20];
	recv(s, rbuf, 20,0);
	string str=(char*)rbuf;
	
	if (str=="free")
	{
		closesocket(s);
		return FREE;
	}
	closesocket(s);
	return BUSY;
}

DWORD WINAPI check_host(LPVOID ip);
DWORD WINAPI refresh_iplist(LPVOID para)
{
	ip_list* obj=(ip_list*)para;
	g_iplist=obj;
	
	char* tmpIP;
	while ((tmpIP=obj->get())!=NULL)
	{
		CreateThread(NULL,0,check_host,tmpIP,0,NULL);
	}
	
	return 0;
}
DWORD WINAPI check_host(LPVOID ip)
{
	const char* ipitem=(const char*)ip;
	ip_state st=isFree(ipitem);//check the status
	g_iplist->setstate(ipitem,st);//set the status
	return 0;
}