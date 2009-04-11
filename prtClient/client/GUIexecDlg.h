// GUIexecDlg.h : ͷ�ļ�
//

#pragma once

#include "file_list.h"
#include "ip_list.h"
//#include "FPRTData.h"
// CGUIexecDlg �Ի���
class ListenInfo
{
	
	ip_list* il;
public:
	ListenInfo(file_list* f=0,ip_list* ip=0)
	{
		pf=f;
		il=ip;
	}
	file_list* getFileList()
	{
		return pf;
	}
	ip_list* getIPList()
	{
		return il;
	}
	void setFileList(file_list* fl)
	{
		pf=fl;
	}
	void setIPList(ip_list* pl)
	{
		il=pl;
	}
	file_list* pf;
};
class CGUIexecDlg : public CDialog
{
	ip_list* m_pServerArray;
	file_list* m_pSceneArray;
	ListenInfo *info;
	
	//FPRTData m_data;
	CEdit m_edit_root;
	CEdit m_edit_dice;
	CEdit m_edit_log;
	CComboBox m_combo_mode;
	CString sRoot;
	int m_dice;
	BOOL i_hdr_direct;
	BOOL i_hdr_indirect;
	BOOL i_hdr_subsurf;
	BOOL i_hdr_scat;
	bool i_do_calculate;
	string m_log;
	
	void initUI();
	void initServerList();
	void doIt();
	void showLog();
	
// ����
public:
	CGUIexecDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_GUIEXEC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
private:
	void UpdateSceneList();
	void UpdateServerList();
	//void UpdateMeshList();

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedExec();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBnLoadPointcloud();
	afx_msg void OnBnClickedBnSetRoot();
	afx_msg void OnBnClickedBnKillPointcloud();
	afx_msg void OnEnChangeEditDice();
	afx_msg void OnEnChangeEditRoot();
	afx_msg void OnBnClickedBtAddServer();
	afx_msg void OnCheckSelectOperation();
};
