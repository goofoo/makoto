// GUIexec.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGUIexecApp:
// �йش����ʵ�֣������ GUIexec.cpp
//

class CGUIexecApp : public CWinApp
{
public:
	CGUIexecApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CGUIexecApp theApp;