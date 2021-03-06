// HustBase.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "HustBase.h"

#include "MainFrm.h"
#include "HustBaseDoc.h"
#include "HustBaseView.h"
#include "TreeList.h"

#include "IX_Manager.h"
#include "PF_Manager.h"
#include "RM_Manager.h"
#include "SYS_Manager.h"

#include <afxwin.h>
#include <string>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHustBaseApp

BEGIN_MESSAGE_MAP(CHustBaseApp, CWinApp)
	//{{AFX_MSG_MAP(CHustBaseApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_CREATEDB, OnCreatDB)	
	ON_COMMAND(ID_OPENDB, OnOpenDB)
	ON_COMMAND(ID_DROPDB, OnDropDb)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHustBaseApp construction

CHustBaseApp::CHustBaseApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHustBaseApp object

CHustBaseApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CHustBaseApp initialization
bool CHustBaseApp::pathvalue=false;

BOOL CHustBaseApp::InitInstance()
{
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CHustBaseDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CHustBaseView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CHustBaseApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CHustBaseApp message handlers

void CHustBaseApp::OnCreatDB()	//需要添加
{
	CString cstr;
	CString cfileName;
	CString filter;
	//filter = "文本文档(*.txt)|*.txt|PDF文档(*.pdf)|*.pdf||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK) {
		cstr = dlg.GetPathName();
		cfileName = dlg.GetFileName();
		AfxMessageBox("数据库创建成功！");
	}
	//CreateDirectory(cstr, NULL);	//str为路径创建文件夹
	char *str = cstr.GetBuffer(cstr.GetLength() + 1);
	char *fileName = cfileName.GetBuffer(cfileName.GetAllocLength() + 1);
	CreateDB(str, fileName);
	//关联创建数据库按钮，此处应提示用户输入数据库的存储路径和名称，
	//并调用CreateDB函数创建数据库。
}

void CHustBaseApp::OnOpenDB()	//需要添加
{
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	LPMALLOC pMalloc;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	
	if (pidl == NULL) return;

	if (pidl != NULL) {
		TCHAR * path = new TCHAR[MAX_PATH];
		SHGetPathFromIDList(pidl, path);
		//      MessageBox(NULL,path,TEXT("Choose"),MB_OK);  
		if (SUCCEEDED(SHGetMalloc(&pMalloc)))//pidl指向的对象用完应该释放，之前忽略了  
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
		OpenDB(path);
		//这是刷新树状结构图的代码
		CHustBaseDoc *pDoc;
		pDoc = CHustBaseDoc::GetDoc();
		CHustBaseApp::pathvalue = true;
		pDoc->m_pTreeView->PopulateTree();

		//AfxMessageBox(_T(path));
		/*m_filePath = path;
		UpdateData(FALSE);*/

		delete[] path;
	}

	//CWinApp::OnFileOpen();
	//关联打开数据库按钮，此处应提示用户输入数据库所在位置，
	//并调用OpenDB函数改变当前数据库路径，并在界面左侧的控件中显示数据库中的表、列信息。
}

void CHustBaseApp::OnDropDb()	//需要添加
{
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	LPMALLOC pMalloc;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	if (pidl == NULL) return;

	if (pidl != NULL) {
		TCHAR * path = new TCHAR[MAX_PATH];
		SHGetPathFromIDList(pidl, path);
		//      MessageBox(NULL,path,TEXT("Choose"),MB_OK);  
		if (SUCCEEDED(SHGetMalloc(&pMalloc)))//pidl指向的对象用完应该释放，之前忽略了  
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
		DropDB(path);
		//

		//AfxMessageBox(_T(path));
		/*m_filePath = path;
		UpdateData(FALSE);*/

		delete[] path;
	}
	//关联删除数据库按钮，此处应提示用户输入数据库所在位置，
	//并调用DropDB函数删除数据库的内容。
}
