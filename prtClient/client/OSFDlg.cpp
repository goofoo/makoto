/*------------------------------------------------------------------
                  COSFDlg class implementation 
              written by Denis Ponomarenko (c) 2007
  ----------------------------------------------------------------*/
//#include "windows.h"
#include "stdafx.h"
#include <commdlg.h>
#include "OSFDlg.h"

// class COSFDialog implementation
/*---------------------------------------------------------
  constructor - OPENFILENAME structure initialization
  ---------------------------------------------------------*/
const TCHAR COSFDialog::szUntitled[100] = TEXT("Untitled\0");

COSFDialog::COSFDialog()
{
	m_ofn.lStructSize       = sizeof(OPENFILENAME);
	m_ofn.hwndOwner         = NULL;
	m_ofn.hInstance         = NULL;
	m_ofn.lpstrFilter       = NULL;            // Set in Open and Save functions
	m_ofn.lpstrCustomFilter = NULL;
	m_ofn.nMaxCustFilter    = 0;
	m_ofn.nFilterIndex      = 0 ;
    m_ofn.lpstrFile         = m_szFileName;   // Set to member variable address      
    m_ofn.nMaxFile          = MAX_PATH * 100 ;
    m_ofn.lpstrFileTitle    = m_szTitleName;   // Set to member variable address 
    m_ofn.nMaxFileTitle     = MAX_PATH * 100;
    m_ofn.lpstrInitialDir   = NULL ;
    m_ofn.lpstrTitle        = NULL ;           // Set in Open and Save functions
    m_ofn.Flags             = NULL ;   
    m_ofn.nFileOffset       = 0 ;
    m_ofn.nFileExtension    = 0 ;
    m_ofn.lpstrDefExt       = NULL ;          // Set in Open and Save functions
    m_ofn.lCustData         = 0 ;
    m_ofn.lpfnHook          = NULL ;
    m_ofn.lpTemplateName    = NULL ;
}

/*------------------- destructor -------------------------*/
COSFDialog::~COSFDialog()
{
}

/*------------ FileOpenDlg function ----------------------*/
BOOL COSFDialog::FileOpenDlg(PTSTR szFilter, PTSTR szDefExt, PTSTR szDlgTitle, BOOL fMultSel)
{
	m_szFileName[0] = m_szTitleName[0] = (TCHAR)'\0';
	m_ofn.lpstrFilter = szFilter;
	m_ofn.lpstrDefExt = szDefExt;
	m_ofn.lpstrTitle  = szDlgTitle;
	if(fMultSel)  // allows multiple file selection
		m_ofn.Flags = OFN_EXPLORER |OFN_ALLOWMULTISELECT;
		
	return GetOpenFileName(&m_ofn);
}

/*-------------------- FileSaveDlg function -------------------*/
BOOL COSFDialog::FileSaveDlg(PTSTR szFilter, PTSTR szDefExt, PTSTR szDlgTitle)
{
	m_szFileName[0] = m_szTitleName[0] = (TCHAR)'\0';
	for(unsigned int i = 0; i <= 10; i++)
		m_szFileName[i] = m_szTitleName[i] = szUntitled[i];
	m_ofn.lpstrFilter = szFilter;
	m_ofn.lpstrDefExt = szDefExt;
	m_ofn.lpstrTitle  = szDlgTitle;
	m_ofn.Flags       = OFN_OVERWRITEPROMPT;

	return GetSaveFileName(&m_ofn);
}

/*---------------------------------------------------------------------------
        GetNextFileName - function that returns results of
		          multiple selection one by one
  ---------------------------------------------------------------------------*/

PCTSTR COSFDialog::GetNextFileName()
{
	static TCHAR szResult[MAX_PATH];
	static TCHAR szDirectory[MAX_PATH];
	static int   iPosition = 0;
	static BOOL bFirstTime = TRUE;
	static BOOL bLastName  = FALSE;
	TCHAR        szFileName[MAX_PATH];
	
	if(bFirstTime)
	{
		iPosition = 0;
		for(unsigned int i = 0; 
			i < m_ofn.nFileOffset; 
		    i++, iPosition++)
		{
		    if((szDirectory[i] = m_szFileName[iPosition]) == TCHAR('\0'))
			{
				if(szDirectory[iPosition - 1] != TCHAR('\\'))
					szDirectory[iPosition] = TCHAR('\\');
			    break;
			}
	    }
		bFirstTime=FALSE;
	}
	if(bLastName)
	{
		bLastName = FALSE;
		bFirstTime = TRUE;
	    return TCHAR('\0');
	}
	if(m_szFileName[iPosition] == TCHAR('\0'))
	    iPosition++;
	szFileName[0] = TCHAR('\0');

	for(unsigned int j = 0; 
		j < sizeof(szFileName)/sizeof(szFileName[0]); 
		j++, iPosition++)
	{
		if((szFileName[j] = m_szFileName[iPosition]) == TCHAR('\0'))
		{
			if(m_szFileName[iPosition + 1] == TCHAR('\0'))
			{
				bLastName = TRUE;
			}
		
			for (int k=0; k<sizeof(szFileName)/sizeof(szFileName[0]);k++)
			{
				if ((szResult[k]=szFileName[k])==TCHAR('\0'))
				{
					break;
				}
			}
			return szResult;
		}
	}
	return TCHAR('\0');
}