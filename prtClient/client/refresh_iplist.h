#ifndef REFRESH_IPLIST_H
#define REFRESH_IPLIST_H
#include "stdafx.h"
#include "ip_list.h"
ip_state isFree(const char* ip);
DWORD WINAPI refresh_iplist(LPVOID para);
#endif //REFRESH_IPLIST_H