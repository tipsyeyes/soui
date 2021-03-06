// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <core/Swnd.h>
#include <control/SCmnCtrl.h>
#include "Dialog/DlgCreatePro.h"
#include "DesignerView.h"
#include "SMoveWnd.h"
#include "propgrid/SPropertyGrid.h"
//#include "STabCtrl2.h"
#include "SImageBtnEx.h"
#include "SListBoxDrop.h"
#include "ResManger.h"

extern SStringT g_CurDir;


class CMainDlg : public SHostWnd
{
public:
	CMainDlg();
	~CMainDlg();

	void OnClose();
	void OnMaximize();
	void OnRestore();
	void OnMinimize();
	void OnSize(UINT nType, CSize size);

	void OnBtnMsgBox();
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

	void OpenProject(SStringT strFileName);
	void CloseProject();

	void OnBtnOpen(); //打开工程
	void OnBtnClose();
	void OnBtnSaveAll();
	void OnBtnSaveLayout(); //保存布局
	void OnBtnNewDialog(); //新建Dialog
	void OnBtnNewInclude(); //新建Include
	void OnBtnWndLayout();
	void OnbtnPreview();
	void OnBtnZYGL();
	void OnBtnYSGL();
	void OnBtnAbout();

	// 直接修改XML文件后加载工程
	void LoadWorkSpace();

	bool OnTreeItemDbClick(EventArgs *pEvtBase);
	bool OnLbControlSelChanged(EventArgs *pEvtBase);

	bool OnDesinerTabSelChanged(EventArgs *pEvtBase);
	bool OnWorkspaceTabSelChanged(EventArgs *pEvtBase);
	bool OnWorkspaceXMLDbClick(EventArgs *pEvtBase);

	void OnScintillaSave(CScintillaWnd * pObj, int custom_msg, SStringT str);

protected:
	void OnLanguage(int nID);
	void OnLanguageBtnCN();
	void OnLanguageBtnJP();
	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_NAME_COMMAND(L"zh_cn", OnLanguageBtnCN)
		EVENT_NAME_COMMAND(L"jp", OnLanguageBtnJP)

		EVENT_NAME_COMMAND(L"toolbar_btn_Open", OnBtnOpen)
		EVENT_NAME_COMMAND(L"toolbar_btn_Close", OnBtnClose)
		EVENT_NAME_COMMAND(L"toolbar_btn_NewDialog", OnBtnNewDialog)
		EVENT_NAME_COMMAND(L"toolbar_btn_NewInclude", OnBtnNewInclude)
		EVENT_NAME_COMMAND(L"toolbar_btn_SaveAll", OnBtnSaveAll)
		EVENT_NAME_COMMAND(L"toolbar_btn_SaveLayout", OnBtnSaveLayout)
		EVENT_NAME_COMMAND(L"toolbar_btn_ZYGL", OnBtnZYGL)
		EVENT_NAME_COMMAND(L"toolbar_btn_YSGL", OnBtnYSGL)
		EVENT_NAME_COMMAND(L"toolbar_btn_YL", OnbtnPreview)
		EVENT_NAME_COMMAND(L"toolbar_btn_about", OnBtnAbout)

		EVENT_NAME_COMMAND(L"uidesigner_wnd_layout", OnBtnWndLayout)
		
	EVENT_MAP_END()

		//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()


private:
	BOOL			m_bLayoutInited;

public:
	SButton* btn_new;
	STreeCtrl * m_treePro;			//工程Layout列表
	SListBox* m_lbWorkSpaceXml;		//工程中的XML文件列表

	SButton* btn_save;

	ResManger m_UIResFileMgr;	// 管理编辑的UI文件资源

	//SList<SDesignerView*> m_ViewList;
	SDesignerView *m_pDesignerView;

	SWindow *m_pLayoutContainer;

	//加载控件列表
	pugi::xml_document xmlDocCtrl;
	SMap<SStringT, pugi::xml_node> m_mapCtrlList;

	SListBoxDrop* m_lbControl;		//控件列表框
	STreeCtrl* m_treeXmlStruct;		//当前编辑窗口的控件结构
	SStatic* m_textNodenum;			//当前编辑窗口的控件数量
	SStatic* m_textCurXmlFile;		//当前XML编辑窗口的文件名显示

	STabPage*	m_pageEditor;		//编辑器的Page页
//	SPropertyGrid *m_pPropgrid;

	//SMap<SStringT, pugi::xml_node> m_mapCommonProperty; //属性通用样式列表   <"skin", xmlnode> <"pos", xmlNode>

	//SMap<SStringT, pugi::xml_node> m_mapProperty;  //控件属性列表 <"button",xmlnode>  <"checkbox",xmlnode>

	SStringT m_strUiresPath;	//uires.idx 的全路径
	SStringT m_strProPath;
	STabCtrl *m_pPropertyPage;
	SWindow* m_wndPropContainer;  //属性控件的容器

	SRichEdit* m_edtDesc; //属性描述

	SRealWnd *m_RealWndLayoutEdit;
	SRealWnd *m_RealWndXmlFile;

	STabCtrl *m_tabDesigner;
	STabCtrl *m_tabWorkspace;
	STabCtrl *m_tabRight;

	SImageBtnEx *m_btnPreview;
	SScrollView *m_scrView;
	BOOL m_bIsOpen;  //工程是否打开
};
