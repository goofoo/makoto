// GUIexecDlg.cpp 
//

#include "stdafx.h"
#include "GUIexec.h"
#include "GUIexecDlg.h"
#include "OSFDlg.h"
#include "FolderDlg.h"
#include "ip_list.h"
#include "refresh_iplist.h"
#include <winsock.h>
#include <windows.h>
#include <string>
#include <iostream>

#include "../../shared/zGlobal.h"
#include "../../shared/XMLUtil.h"
using namespace std;

char global_prtgen_cmd[256] = "prtServer";
char global_log[256] = "D:/prtclient.log";

string global_log_str;

void saveLog(const char* text, const char* addr)
{
	ofstream ffile;
	ffile.open(global_log, ios::out | ios::app);
	
	string log; 
	zGlobal::getTime(log);
	log += text;
	log += addr;
	log += "\n";
	
	ffile.write ((char*)log.c_str(), log.size());
	
	ffile.close();
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


class Initsock
{
public:
	Initsock()
	{
		WSADATA data;
		WSAStartup(MAKEWORD(2,2),&data);
	}
	~Initsock()
	{
		WSACleanup();
	}
};
Initsock it;
class NetworkErr
{
	char* m_str;
public:
	NetworkErr(char* s){m_str=s;}
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

/*Implementation on specific server*/
bool sendTaskTo(const char* ip, const char* argument)
{
	SOCKET s=socket(AF_INET,SOCK_STREAM,0);
	sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(2000);
	addr.sin_addr.S_un.S_addr=inet_addr(ip);
	if (connect(s, (SOCKADDR*)&addr, sizeof(addr))!=0)
	{
		closesocket(s);
		return false;
	}
	send(s, "command",12,0);
	char rbuf[20];
	recv(s,rbuf,20,0);
	string str=(char*)rbuf;

	if (str!="command ok")
	{
		closesocket(s);
		return false;
	}
	
	str = string(global_prtgen_cmd) + string(argument);

	send(s, str.c_str(), str.length()+1, 0);
	closesocket(s);
	
	//saveLog(" sent job to ", ip);
	
	return true;
}

CriticalSection cs1;

DWORD WINAPI listenThread(LPVOID para)
{
	ListenInfo *info=(ListenInfo*)para;
	SOCKET s=socket(AF_INET,SOCK_STREAM,0);
	sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(888);
	addr.sin_addr.S_un.S_addr=INADDR_ANY;
	bind(s, (SOCKADDR*)&addr, sizeof(addr));
	listen(s,5);
	SOCKET dt;
	char buf[100];
	string str;
	char* p;
	string filename;
	sockaddr_in remote;
	char* ip;
	int retl=sizeof(SOCKADDR);
	while(true)
	{
		dt=accept(s, (SOCKADDR*)&remote, &retl);
		ip=inet_ntoa(remote.sin_addr);
		recv(dt, buf, 100, 0);
		str=(char*)buf;
		if (str=="file")
		{
			cs1.lock();
			p=info->getFileList()->get();

			if (NULL!=p)
			{
				str=p;
				Sleep(1999);
				send(dt, str.c_str(), str.length()+1, 0);
			}
			else
			{
				send(dt, "no left", 8, 0);
				recv(dt, buf, 100, 0);
				str=(char*)buf;
				if (str=="all done") info->getIPList()->setstate(ip, FREE);
			}
			cs1.unlock();
		}
		else if (str=="result")
		{
			saveLog(" received work from ", ip);
			send(dt, "result",7,0);
			recv(dt,buf,100,0);
			istringstream in(buf);
			in>>filename>>str;
			
			char strfilename[512], sstat[16];
			int res = sscanf(buf, "%s %s", strfilename, sstat);
			cs1.lock();
			info->pf->end(strfilename);

			cs1.unlock();
			send(dt, "result ok", 8,0);
		}
		closesocket(dt);
	}
	closesocket(s);
}
/*About Dialog*/
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX); 

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


/*Main Dialog*/
CGUIexecDlg::CGUIexecDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGUIexecDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pSceneArray=NULL;
	m_pServerArray=new ip_list();
	info=new ListenInfo();
	i_do_calculate = 0;
}

void CGUIexecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ROOT, m_edit_root);
	DDX_Control(pDX, IDC_EDIT_DICE, m_edit_dice);
	DDX_Control(pDX, IDC_EDIT_LOG, m_edit_log);
	DDX_Check(pDX, IDC_CHECK_HDR_DIRECT, i_hdr_direct);
	DDX_Check(pDX, IDC_CHECK_HDR_INDIRECT, i_hdr_indirect);
	DDX_Check(pDX, IDC_CHECK_HDR_SUBSURF, i_hdr_subsurf);
	DDX_Check(pDX, IDC_CHECK_HDR_SCAT, i_hdr_scat);
}

BEGIN_MESSAGE_MAP(CGUIexecDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_EXEC, &CGUIexecDlg::OnBnClickedExec)
	ON_WM_TIMER()

	ON_BN_CLICKED(IDC_BN_LOAD_POINTCLOUD, &CGUIexecDlg::OnBnClickedBnLoadPointcloud)
	ON_BN_CLICKED(IDC_BN_SET_ROOT, &CGUIexecDlg::OnBnClickedBnSetRoot)
	ON_BN_CLICKED(IDC_BN_KILL_POINTCLOUD, &CGUIexecDlg::OnBnClickedBnKillPointcloud)
	ON_EN_CHANGE(IDC_EDIT_DICE, &CGUIexecDlg::OnEnChangeEditDice)
	ON_EN_CHANGE(IDC_EDIT_ROOT, &CGUIexecDlg::OnEnChangeEditRoot)
	ON_BN_CLICKED(IDC_BT_ADD_SERVER, &CGUIexecDlg::OnBnClickedBtAddServer)
	ON_BN_CLICKED(IDC_CHECK_HDR_DIRECT, OnCheckSelectOperation)
	ON_BN_CLICKED(IDC_CHECK_HDR_INDIRECT, OnCheckSelectOperation)
	ON_BN_CLICKED(IDC_CHECK_HDR_SUBSURF, OnCheckSelectOperation)
	ON_BN_CLICKED(IDC_CHECK_HDR_SCAT, OnCheckSelectOperation)
	ON_BN_CLICKED(IDC_CHECK_LIGHTSOURCE_DIRECT, OnCheckSelectOperation)
	ON_BN_CLICKED(IDC_CHECK_LIGHTSOURCE_INDIRECT, OnCheckSelectOperation)
	ON_BN_CLICKED(IDC_CHECK_LIGHTSOURCE_SUBSURF, OnCheckSelectOperation)
	ON_BN_CLICKED(IDC_CHECK_LIGHTSOURCE_SCAT, OnCheckSelectOperation)
END_MESSAGE_MAP()

/*Initialization to read setup file*/
BOOL CGUIexecDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	
	initUI();
	initServerList();
		
	
	
	CreateThread(NULL,0,listenThread,(LPVOID)info,0,0);
	
	SetTimer(1,1000,NULL);
	return TRUE;
}

void CGUIexecDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CGUIexecDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CGUIexecDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*Do nothing when user press enter key*/
void CGUIexecDlg::OnOK() {}
	
void CGUIexecDlg::initUI()
{
	m_edit_root.SetWindowText ( "Not Set" );
	m_edit_dice.SetWindowText ( "4" );
	m_dice = 4;

	string log;
	zGlobal::getTime(log);
	log += " PRT Client started";
	m_edit_log.SetWindowText ( log.c_str() );
	m_log = log+"\r\n";
}


void CGUIexecDlg::initServerList()
{
	ifstream in("prtclient.ini");
	string str;
	int mode;
	while(getline(in,str))
	{
		if(str.compare("#list address of available servers below")==0)
		{
			mode = 0;
			continue;
		}
		else if(str.compare("#set shared folder as root path")==0)
		{
			mode = 1;
			continue;
		}
		
		if(mode==0 && str.size() > 2 && str[0] != '#')
		{
			m_pServerArray->append(str.c_str());
			
			ip_state st=isFree(str.c_str());
			
			if (st==FREE)
				m_pServerArray->setstate(str.c_str(),FREE);
			else if (st==BUSY)
				m_pServerArray->setstate(str.c_str(),BUSY);
			else
				m_pServerArray->setstate(str.c_str(),NOTCONNECT);
		}
		else if(mode==1 && str.size() > 2 && str[0] != '#')
		{
			if(str[str.size()-1] != '\\') str.append(1,'\\');
			m_edit_root.SetWindowText(str.c_str());
			//m_data.setRoot(str.c_str());
		}
	}
	CListCtrl* list_servers =(CListCtrl*)GetDlgItem(IDC_IP_LIST);
	list_servers->SetExtendedStyle(LVS_EX_CHECKBOXES);
	
	char* server_name;
	CString label;
	while((server_name=m_pServerArray->get())!=NULL)
	{
		label = server_name;
		if (m_pServerArray->getstate(server_name)==FREE)
		{
			label+=" (idle)";
			list_servers->InsertItem(0, label);
			list_servers->SetCheck(0,1);
		}
		else if (m_pServerArray->getstate(server_name)==BUSY)
		{
			label+=" (busy)";
			list_servers->InsertItem(0, label);
			list_servers->SetCheck(0,1);
		}
		else
		{
			label+=" (lost)";
			list_servers->InsertItem(0, label);
			list_servers->SetCheck(0,0);
		}
	}
	
	info->setIPList(m_pServerArray);
	
}

void CGUIexecDlg::UpdateSceneList()
{
	if(m_pSceneArray==NULL) return;
	if (!m_pSceneArray->IsModified()) return;
	
	CListBox* p=(CListBox*)GetDlgItem(IDC_FILE_LIST);
	
	char* str;
	string prefix;
	p->ResetContent();
	while(true)
	{
		str=m_pSceneArray->list();
		if (str==NULL) break;
		prefix="";
		switch(m_pSceneArray->getstate(str))
		{
		case QUEUING:
			break;
		case PROCESSING:
			prefix="(processing)";
			break;
		default:
			break;
		}
		string s=prefix+str;
		p->AddString(s.c_str());
	}
}

void CGUIexecDlg::UpdateServerList()
{
	CListCtrl* list_servers=(CListCtrl*)GetDlgItem(IDC_IP_LIST);
// get selected servers before refreshing	
	int n_server = list_servers->GetItemCount();
	for (int i=0; i<n_server; i++)
	{
		string sname = list_servers->GetItemText(i,0);
		zGlobal::trimByFirstSpace(sname);
		if (list_servers->GetCheck(i)) 
		{
			m_pServerArray->selectServer(sname.c_str());
			global_log_str = "select "+sname;
		}
		else 
		{
			m_pServerArray->unselectServer(sname.c_str());
			global_log_str = "unselect "+sname;
		}
	}
	
	if (!m_pServerArray->stateModified()) return;
// refresh	
	list_servers->DeleteAllItems();
	
	char* server_name;
	CString label;
	while((server_name=m_pServerArray->get())!=NULL)
	{
		label = server_name;
		if (m_pServerArray->getstate(server_name)==FREE)
		{
			label+=" (idle)";
		}
		else if (m_pServerArray->getstate(server_name)==BUSY)
		{
			label+=" (busy)";
		}
		else
		{
			label+=" (lost)";
		}
//update label
		list_servers->InsertItem(0, label);
// update check		
		if(m_pServerArray->isServerSelected(server_name)) list_servers->SetCheck(0,1);
		else list_servers->SetCheck(0,0);
	}
}

void CGUIexecDlg::doIt()
{
// wait for last scenes
	if(m_pSceneArray)
	{
		if(!m_pSceneArray->isQueuing() && m_pSceneArray->isQueuingOrProcessing()) return;
	}

	int i_has_scene = 0;
	if(m_pSceneArray) if(m_pSceneArray->isQueuingOrProcessing()) i_has_scene = 1;
	
	if(i_has_scene == 0)
	{
		global_log_str = string(" All Task Sent to Servers");
		showLog();
		i_do_calculate = 0;
		return;
	}
	
	info->pf=m_pSceneArray;
	
	int iopt[4];
	if(i_hdr_direct) iopt[0]=1;
	else iopt[0] = 0;
	if(i_hdr_indirect) iopt[1]=1;
	else iopt[1] = 0;
	if(i_hdr_subsurf) iopt[2]=1;
	else iopt[2] = 0;
	if(i_hdr_scat) iopt[3]=1;
	else iopt[3] = 0;
	
	//char logb[32];
	//sprintf(logb, "ss %i %i %i %i %i %i", iopt[0],  iopt[1],  iopt[2],  iopt[3],  iopt[4],  iopt[5]);
	//global_log_str = string(logb);
	//showLog();
	
//	info->pf->setOperation(iopt);
	
	char argv[640];
	sprintf(argv, " -path %s -dice %d -operation %d %d %d %d ", sRoot.GetBuffer(sRoot.GetLength()), m_dice, iopt[0],  iopt[1],  iopt[2],  iopt[3]);

	for(int i=0; i<m_pServerArray->getNumIterm(); i++)
	{
		char* server_name = m_pServerArray->getIterm(i);

		if(m_pServerArray->getstate(server_name) == FREE && m_pServerArray->isServerSelected(server_name)) 
		{
			sendTaskTo(server_name, argv);
			global_log_str = string(" Sent task to ") + string(server_name);
			showLog();
		}
	}
}

void CGUIexecDlg::OnBnClickedExec()
{
	i_do_calculate = 1;
}

void CGUIexecDlg::showLog()
{
	string log;
	zGlobal::getTime(log);
	log += global_log_str;
	m_log += log;
	m_edit_log.SetWindowText ( m_log.c_str() );
	m_log += "\r\n";
}

void CGUIexecDlg::OnTimer(UINT_PTR nIDEvent)
{	
	UpdateSceneList();
	UpdateServerList();
	
	CreateThread(NULL,0, refresh_iplist, m_pServerArray,0,NULL);
	
	if(i_do_calculate) doIt();
	
	CDialog::OnTimer(nIDEvent);
}


void CGUIexecDlg::OnBnClickedBnLoadPointcloud()
{

	TCHAR szFilter[] = TEXT ("Scene Files (*.scene)\0*.scene\0")  \
                               TEXT ("All Files (*.*)\0*.*\0\0");
	TCHAR szDefExtention[] = TEXT("scene\0");

	static COSFDialog osfDlg;

	
	if(osfDlg.FileOpenDlg(szFilter, szDefExtention,TEXT("SCENE FILE:)"), TRUE))
	{
		CListBox* m_pSceneArrayList=(CListBox*)GetDlgItem(IDC_FILE_LIST);
		
		if(!m_pSceneArray) m_pSceneArray=new file_list();
		
		if(osfDlg.IsMultiSelect())
		{
			LPCTSTR str, last_name;
			while((str = osfDlg.GetNextFileName()) != TCHAR('\0'))
			{
				last_name= str;
				m_pSceneArray->add(last_name);
				
				//global_log_str = string(" add ")+string(last_name);
				//showLog();
			}
			string pcfname(last_name);
			int firstdot = pcfname.find_first_of('.', 0);
			if(firstdot > 1) pcfname.erase(firstdot);
	
			//m_edit_pcf.SetWindowText (CString(pcfname.c_str()));
			//m_data.setPCF(pcfname.c_str());

			UpdateSceneList();
			
			int n_scene = m_pSceneArray->getNumIterm();
			char sbuf[256];
			sprintf(sbuf, " Append %i scene files", n_scene);
			global_log_str = string(sbuf);
			showLog();
		}
		
	}
}

void CGUIexecDlg::OnBnClickedBnSetRoot()
{

	LPCTSTR lpszTitle = _T( "Select the root folder for the PRT data files:" );
	UINT	uFlags	  = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	CString			m_strRoot;
	CFolderDialog dlgRoot( lpszTitle, m_strRoot, this, uFlags );
		
	if( dlgRoot.DoModal() == IDOK )
	{
		sRoot = CString(dlgRoot.GetFolderPath());
		m_edit_root.SetWindowText (sRoot);
		//m_data.setRoot(sRoot.GetBuffer(sRoot.GetLength()));
	}
}

void CGUIexecDlg::OnBnClickedBnKillPointcloud()
{
	CListBox* m_pSceneArrayList=(CListBox*)GetDlgItem(IDC_FILE_LIST);
	m_pSceneArrayList->ResetContent();
	if (m_pSceneArray!=NULL) 
	{
		delete m_pSceneArray;
		m_pSceneArray=NULL;
	}
	global_log_str = string(" Remove all scene files ");
	showLog();
	UpdateSceneList();
}

void CGUIexecDlg::OnEnChangeEditDice()
{
	CString stmp;
	m_edit_dice.GetWindowText(stmp);
	//m_data.setDice(stmp.GetBuffer(stmp.GetLength()));
	m_dice = atoi(stmp.GetBuffer(stmp.GetLength()));
}

void CGUIexecDlg::OnEnChangeEditRoot()
{
	m_edit_root.GetWindowText (sRoot);
	//m_data.setRoot(sRoot.GetBuffer(sRoot.GetLength()));
}

void CGUIexecDlg::OnBnClickedBtAddServer()
{
	CIPAddressCtrl* pIPCtrl = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS);
	if(!pIPCtrl->IsBlank())
	{
		DWORD dwIP;
		pIPCtrl->GetAddress(dwIP);
		in_addr addr;
		addr.S_un.S_addr=htonl(dwIP);
		char* ipstr=inet_ntoa(addr);
		ip_state st=isFree(ipstr);
		if (m_pServerArray->exist(ipstr))
		{
			m_pServerArray->setstate(ipstr,st);
		}
		else
		{
			if (st==FREE)
			{
				m_pServerArray->append(ipstr);
				m_pServerArray->setstate(ipstr,FREE);
			}
		}
		UpdateServerList();
	}
}

void CGUIexecDlg::OnCheckSelectOperation()
{
	UpdateData();
}
//:~