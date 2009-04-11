// server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <sstream>
#include <stdio.h>
#include <conio.h>
#include <fstream>
#include <iostream>

using namespace std;

#include "../../shared/zGlobal.h"

class InitSock
{
public:
	InitSock()
	{
		WSADATA dt;
		WSAStartup(MAKEWORD(2,2),&dt);
	}
	~InitSock()
	{
		WSACleanup();
	}
};

class CriticalSection			
{
	CRITICAL_SECTION cs;
public:
	CriticalSection()
	{
		InitializeCriticalSection(&cs);
	}
	~CriticalSection()
	{
		DeleteCriticalSection(&cs);
	}
	void lock()
	{
		EnterCriticalSection(&cs);
	}
	void unlock()
	{
		LeaveCriticalSection(&cs);
	}
};

DWORD WINAPI threadFunc(LPVOID para);
InitSock it;
CriticalSection cs;
//bool bFree=true;
int nThreads=0;
int nMaxThreads=0;

struct ExecInfo
{
	char* m_ip;
	char* m_cmd;
	char* argument;
};

bool ExecProgram(char* exeName)
{
	STARTUPINFO si={sizeof(si)};
	PROCESS_INFORMATION pi;
	int ret;
	cout<<exeName<<endl;
	ret=CreateProcess(NULL,exeName,NULL,NULL,false,CREATE_NEW_CONSOLE,NULL,NULL,&si,&pi);
	cout<<"<Process started...";
	WaitForSingleObject(pi.hProcess,INFINITE);
	if(ret==0) return false;
	cout<<"ended>"<<endl;
	return true;
}

DWORD WINAPI threadFunc(LPVOID param)
{
	ExecInfo* info=(ExecInfo*)param;

	cs.lock();

	cs.unlock();
	SOCKET s;
	sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(888);
	addr.sin_addr.S_un.S_addr=inet_addr(info->m_ip);

	string filename;
	bool bExec;
	while (true)
	{
		s=socket(AF_INET,SOCK_STREAM,0);
		if(connect(s,(SOCKADDR*)&addr,sizeof(addr))==SOCKET_ERROR) break;
		send(s,"file",5,0);
		char buf[512];
		recv(s,buf,512,0);
		
		string str((char*)buf);

		filename=str;
		if (str== "no left" )
		{
			cs.lock();
			// if this the last thread, send the all done flag to client
			if (nThreads==1)
				send(s, "all done",9,0);
			else
				send(s, "not done", 9,0);
			cs.unlock();
			closesocket(s);
			break;
		}
		else
		{
			closesocket(s);
			str=info->argument+str;

			string result;
			result=info->m_cmd;
			result+=' ';
			result+=str;
			strcpy(buf,result.c_str());
//Call workforce
			bExec=ExecProgram(buf);
		}
		// send program execution result
		s=socket(AF_INET,SOCK_STREAM,0);
		connect(s,(SOCKADDR*)&addr,sizeof(addr));
		send(s,"result",7,0);
		recv(s,buf,256,0);
		if (bExec){
			filename+=' ';
			filename+="okay";
		}
		else
		{
			filename+=' ';
			filename+="fail";
		}
		send(s,filename.c_str(),filename.length()+1,0);
		recv(s,buf,256,0);
		closesocket(s);
	}
	cs.lock();
	nThreads--;
	//if (nThreads==0)
	//	bFree=true;
	cs.unlock();
	return 0;
}
void ReadThreadsFile()
{
	ifstream in("remotecmdserver.ini");
	string str;
	getline(in,str);
	if (str=="")
		nMaxThreads=1;
	else
		nMaxThreads=atoi(str.c_str());
}
int main(int argc, char * const argv[])
{
	cout<<"Remote Command Server 1.2.4 04/07/09 "<<endl;
	HANDLE hSingle;
	hSingle=CreateMutex(NULL,TRUE,"CmdServer");
	if (GetLastError()==ERROR_ALREADY_EXISTS)
	{
		return -1;
	}
	
	if(argc ==3 && strcmp(argv[1], "-p") ==0)	 
	{
		nMaxThreads = atoi(argv[2]);
	}
	else ReadThreadsFile();
	
	cout<<"N Thread:"<<nMaxThreads <<endl;
	SOCKET s=socket(AF_INET,SOCK_STREAM,0);
	if(s!=INVALID_SOCKET) printf("socket successfully created!\n");

	sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(2000);
	local.sin_addr.S_un.S_addr=INADDR_ANY;
	int rc = bind(s,(SOCKADDR*)&local,sizeof(SOCKADDR));
	if(rc == SOCKET_ERROR)
	{
		printf("bind failed:%d\n",::WSAGetLastError());
		return -1;
	}
	
	listen(s,5);
	
	SOCKET dt;
	sockaddr_in remote;
	int i=sizeof(SOCKADDR);
	char* ip;
	char* pBuf=new char[1024];
	while(TRUE)
	{
		dt=accept(s,(SOCKADDR*)&remote,&i);
		recv(dt,pBuf,1024,0);
		string str=pBuf;
		ip=inet_ntoa(remote.sin_addr);
		if (str=="host free")
		{
			//cout<<"reply to client "<<ip<<": server status is ";
			cs.lock();
			//if (bFree)
			if(nThreads<nMaxThreads)
			{	
				//cout<<"idle\n";
				send(dt,"free",5,0);
			}
			else
			{	
				//cout<<"busy\n";
				send(dt,"busy",5,0);
			}
			cs.unlock();
		}
		else if(str=="command")//receive command
		{
			cout<<"receive command from client "<<ip<<" : ";
			cs.lock();
			//if (bFree==true)
			if(nThreads<nMaxThreads)
			{
				send(dt,"command ok",11,0);
//Input
				recv(dt,pBuf,1024,0);
				string cmd = pBuf;
				string argument;
				
				zGlobal::divideByFirstSpace(cmd, argument);
				
				ExecInfo* param=new ExecInfo;
				param->argument=new char[argument.length()+1];
				strcpy(param->argument,argument.c_str());
				param->m_cmd=new char[cmd.length()+1];
				strcpy(param->m_cmd,cmd.c_str());
				param->m_ip=new char[strlen(ip)+1];
				strcpy(param->m_ip,ip);

//Threading				
				//bFree=false;
				while (nThreads<nMaxThreads)
				{
					nThreads++;
					CreateThread(NULL,0,threadFunc,(LPVOID)param,0,0);
					Sleep(2000);
				}
				
			}
			cs.unlock();
		}
		closesocket(dt);
	}
	delete pBuf;
	closesocket(s);
	
	WSACleanup();
	ReleaseMutex(hSingle);
	cout<<"Remote server stopped.";
	return 0;
}
//:~
