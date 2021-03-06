// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"
#include "FileHelper.h"
#include "LogParser.h"
#include <helper/SMenu.h>

#ifdef DWMBLUR	//win7毛玻璃开关
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")
#endif


CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND")),m_lvLogs(NULL),m_cbxLevels(NULL)
{
	m_logAdapter.Attach(new SLogAdapter);
	IParserFactory * pParserFactory = new CParseFactory;
	m_logAdapter->SetParserFactory(pParserFactory);
	pParserFactory->Release();
}

CMainDlg::~CMainDlg()
{
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	#ifdef DWMBLUR	//win7毛玻璃开关
	MARGINS mar = {5,5,30,5};
	DwmExtendFrameIntoClientArea ( m_hWnd, &mar );
	#endif

	SetMsgHandled(FALSE);
	return 0;
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{

	//设置为磁吸主窗口
	SetMainWnd(m_hWnd);

	m_pFilterDlg = new CFilterDlg(this);
	m_pFilterDlg->Create(m_hWnd);
	AddSubWnd(m_pFilterDlg->m_hWnd,AM_LEFT,AA_TOP);

	m_pFilterDlg->ShowWindow(SW_SHOW);

	m_lvLogs = FindChildByID2<SMCListView>(R.id.lv_log);
	if(m_lvLogs)
	{
		m_lvLogs->SetAdapter(m_logAdapter);
	}

	m_cbxLevels = FindChildByID2<SComboBox>(R.id.cbx_levels);
	
	::RegisterDragDrop(m_hWnd,GetDropTarget());

	IDropTarget *pDT = new CDropTarget(this);
	GetContainer()->RegisterDragDrop(m_lvLogs->GetSwnd(),pDT);
	pDT->Release();

	SRealWnd * pRealWnd = FindChildByID2<SRealWnd>(R.id.real_scilexer);
	SASSERT(pRealWnd);
	m_logAdapter->SetScintillaWnd((CScintillaWnd *)pRealWnd->GetUserData());
	return 0;
}

void CMainDlg::OnLanguageBtnCN()
{
	OnLanguage(_T("lang_cn"));
}
void CMainDlg::OnLanguageBtnEN()
{
	OnLanguage(_T("lang_en"));
}

void CMainDlg::OnLanguage(const SStringT & strLang)
{
	ITranslatorMgr *pTransMgr = SApplication::getSingletonPtr()->GetTranslator();
	SASSERT(pTransMgr);

	pugi::xml_document xmlLang;
	if (SApplication::getSingletonPtr()->LoadXmlDocment(xmlLang, strLang,_T("languages")))
	{
		CAutoRefPtr<ITranslator> lang;
		pTransMgr->CreateTranslator(&lang);
		lang->Load(&xmlLang.child(L"language"), 1);//1=LD_XML
		pTransMgr->SetLanguage(lang->name());
		pTransMgr->InstallTranslator(lang);
		SDispatchMessage(UM_SETLANGUAGE,0,0);
		m_pFilterDlg->SDispatchMessage(UM_SETLANGUAGE,0,0);
	}
}

void CMainDlg::OnClose()
{
	CSimpleWnd::DestroyWindow();
}

void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	
	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if(!pBtnMax || !pBtnRestore) return;
	
	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
}

void CMainDlg::OnOpenFile()
{
	CFileDialogEx openDlg(TRUE,_T("log"),0,6,_T("log files(*.log)\0*.log\0txt files(*.txt)\0*.txt\0All files (*.*)\0*.*\0\0"));
	if(openDlg.DoModal()==IDOK)
	{
		OpenFile(openDlg.m_szFileName);
	}
}

void CMainDlg::OnFilterInputChange(EventArgs *e)
{
	EventRENotify *e2 = sobj_cast<EventRENotify>(e);
	SASSERT(e2);
	if(e2->iNotify == EN_CHANGE)
	{
		SEdit * pEdit = sobj_cast<SEdit>(e2->sender);
		SStringT str = pEdit->GetWindowText();
		m_logAdapter->SetFilter(str);
	}
}

void CMainDlg::OnLevelsChange(EventArgs *e)
{
	EventCBSelChange * e2 = sobj_cast<EventCBSelChange>(e);
	m_logAdapter->SetLevel(e2->nCurSel);
}


void CMainDlg::OnFileDropdown(HDROP hDrop)
{
	bool success = false;
	TCHAR filename[MAX_PATH];
	success=!!DragQueryFile(hDrop, 0, filename, MAX_PATH);
	if(success) 
	{
		if(!(GetFileAttributes(filename) & FILE_ATTRIBUTE_DIRECTORY))
		{
			OpenFile(filename);
		}
	}

}

void CMainDlg::OpenFile(LPCTSTR pszFileName)
{
	if(!m_logAdapter->Load(pszFileName)) return;

	ILogParse *pLogParser = m_logAdapter->GetLogParse();
	if(pLogParser)
	{
		m_cbxLevels->ResetContent();
		int nLevels = pLogParser->GetLevels();
		wchar_t (*szLevels)[MAX_LEVEL_LENGTH] = new wchar_t[nLevels][MAX_LEVEL_LENGTH];
		pLogParser->GetLevelText(szLevels);
		for(int i=0;i<nLevels;i++)
		{
			m_cbxLevels->InsertItem(i,S_CW2T(szLevels[i]),0,i);
		}
		delete []szLevels;

	}

	SArray<SStringW> lstTags;
	m_logAdapter->GetTags(lstTags);
	m_pFilterDlg->UpdateTags(lstTags);


	SArray<UINT> lstPid;
	m_logAdapter->GetPids(lstPid);
	m_pFilterDlg->UpdatePids(lstPid);

	SArray<UINT> lstTid;
	m_logAdapter->GetTids(lstTid);
	m_pFilterDlg->UpdateTids(lstTid);

}

bool CMainDlg::OnLvContextMenu(CPoint pt)
{
	CPoint pt2 = pt;
	ClientToScreen(&pt2);

	SItemPanel *pItem = m_lvLogs->HitTest(pt);
	if(!pItem) return false;

	int iItem = pItem->GetItemIndex();
	SLogInfo *pLogInfo = m_logAdapter->GetLogInfo(iItem);

	SMenu menu;
	menu.LoadMenu(UIRES.smenu.menu_lv);
	SStringW str = TR(GETSTRING(R.string.exclude_tag),L"");
	str += pLogInfo->strTag;
	menu.ModifyMenuString(101,MF_BYCOMMAND,S_CW2T(str));
	str = TR(GETSTRING(R.string.only_tag),L"");
	str += pLogInfo->strTag;
	menu.ModifyMenuString(102,MF_BYCOMMAND,S_CW2T(str));

	int cmd = menu.TrackPopupMenu(TPM_RETURNCMD,pt2.x,pt2.y,NULL);
	if(cmd==100)
	{
		HGLOBAL hMen;   

		// 分配全局内存    
		hMen = GlobalAlloc(GMEM_MOVEABLE, pLogInfo->strContent.GetLength()*sizeof(WCHAR));    

		if (!hMen)   
		{   
			return false;         
		}   

		LPWSTR lpStr = (LPWSTR)GlobalLock(hMen);    

		// 内存复制   
		memcpy(lpStr, (LPCWSTR)pLogInfo->strContent, pLogInfo->strContent.GetLength()*sizeof(WCHAR));    
		// 释放锁    
		GlobalUnlock(hMen);   

		::OpenClipboard(m_hWnd);
		::SetClipboardData(CF_UNICODETEXT,hMen);
		::CloseClipboard();
	}else if(cmd == 101)
	{
		m_pFilterDlg->ExcludeTag(pLogInfo->strTag);
	}else if(cmd == 102)
	{
		m_pFilterDlg->OnlyTag(pLogInfo->strTag);
	}

	return true;
}

void CMainDlg::UpdateFilterTags(const SArray<SStringW>& lstTags)
{
	m_logAdapter->SetFilterTags(lstTags);
}

void CMainDlg::UpdateFilterPids(const SArray<UINT> & lstPid)
{
	m_logAdapter->SetFilterPids(lstPid);
}

void CMainDlg::UpdateFilterTids(const SArray<UINT> & lstTid)
{
	m_logAdapter->SetFilterTids(lstTid);
}

