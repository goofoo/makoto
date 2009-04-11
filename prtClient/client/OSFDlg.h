/*------------------------------------------------------------------
                     COSFDlg class header 
              written by Denis Ponomarenko (c) 2007
  ----------------------------------------------------------------*/
/*-----------------------------------------------------------------
  class COSFDialog - class that incapsulates work with "open file" 
                   and  "Save As..." dialogs. 
      This class is analog of similar MFC class, but it has no
          MFC dependencies and no parent classes overhead.
   ----------------------------------------------------------------*/
#pragma once

class COSFDialog
{
public:
	COSFDialog();
	virtual ~COSFDialog();

	BOOL FileOpenDlg(PTSTR szFilter, PTSTR szDefExt, PTSTR szDlgTitle, BOOL fMultSel);
	BOOL FileSaveDlg(PTSTR szFilter, PTSTR szDefExt, PTSTR szDlgTitle);

	/*----- retrieving file full name ------------*/
	PCTSTR GetFileName()const { return m_szFileName;} 
	/*----- retrieving file title ----------------*/
	PCTSTR GetTitleName()const { return m_szTitleName; }
	PCTSTR GetNextFileName();
	BOOL   IsMultiSelect(){ return(m_ofn.Flags & OFN_ALLOWMULTISELECT);}



 	// members
private:
    OPENFILENAME  m_ofn;         
	TCHAR m_szFileName[MAX_PATH * 100];
	TCHAR m_szTitleName[MAX_PATH * 100];
    static const TCHAR szUntitled [100];
};



