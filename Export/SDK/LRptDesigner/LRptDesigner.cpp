// LRptDesigner.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "LRptDesigner.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "LRptDesignerDoc.h"
#include "LRptDesignerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CONTENT_FILE	".etl"
#define KEY_NAME		"LExcelFile"

//extern HINSTANCE g_hInstance;
/////////////////////////////////////////////////////////////////////////////
// CLRptDesignerApp

BEGIN_MESSAGE_MAP(CLRptDesignerApp, CWinApp)
	//{{AFX_MSG_MAP(CLRptDesignerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLRptDesignerApp construction

CLRptDesignerApp::CLRptDesignerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLRptDesignerApp object

CLRptDesignerApp theApp;
CLRptDesignerApp*	GetApp()
{
	return &theApp;
}
/////////////////////////////////////////////////////////////////////////////
// CLRptDesignerApp initialization



BOOL CLRptDesignerApp::InitInstance()
{
	
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	m_fontDefault.CreateFontIndirect(&DEFAULT_LOGFONT);
	if(m_pDefaultFont==NULL){
		m_pDefaultFont=&m_fontDefault;
	}
	AfxEnableControlContainer();
#ifdef STATIC_LINK
	g_hInstance=GetApp()->m_hInstance;
#endif
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

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_LRPTDETYPE,
		RUNTIME_CLASS(CLRptDesignerDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CLRptDesignerView));
	AddDocTemplate(pDocTemplate);
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	CMainFrame::AdjustHelpMenu(m_pMainWnd->m_hWnd);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	
	SetFileContent();
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
void CLRptDesignerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CLRptDesignerApp message handlers


void CLRptDesignerApp::SetFileContent()
{
	m_reg.SetRootKey(HKEY_CLASSES_ROOT);
	char strT[MAX_PATH];
	CString strExe;
	LONG nSize=MAX_PATH;
	::GetModuleFileName(GetModuleHandle(NULL),strT,nSize);
	ASSERT(nSize>0);
	strExe=strT;
	strExe+=L" %1";
	CString strV;
	if(!m_reg.GetValue(CONTENT_FILE,strV)){
		m_reg.CreateKey(CONTENT_FILE);
	}
	if(m_reg.SetValue("",KEY_NAME)){
		strV=KEY_NAME;
		strV+=L"\\shell\\open\\command";
		if(!m_reg.CreateKey(strV)){
			ASSERT(FALSE);
		}
		if(!m_reg.SetValue("",strExe)){
			ASSERT(FALSE);
		}
	}

}

CDocument* CLRptDesignerApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	// TODO: Add your specialized code here and/or call the base class
	CDocument* ret=NULL;
	ret=CWinApp::OpenDocumentFile(lpszFileName);
	CLRptDesignerView* pView = GetApp()->GetActiveView();
	if(pView && ret){
		if(!LoadBook(GetBook(pView->GetReport()),(char*)lpszFileName)){
			AfxMessageBox("不可识别的文件格式!");
			return NULL;
		}
		LONG book=GetBook(pView->GetReport());
		LONG ct=GetSheetCount(book);
		LONG sheet=0;
		for(int i=1;i<=ct;i++){
			sheet=GetSheet(book,i);
			if(sheet)
				pView->RefreshFixedCell(sheet);
		}
	}
	return ret;
}
/*
void CLRptDesignerApp::OnFileNew()
{
	CWinApp::OnFileNew();
	CLRptDesignerDoc* pDoc=(CLRptDesignerDoc*)GetActiveDoc();
	if(pDoc){
		//CLRptDesignerDoc::AdjustHelpMenu(pView->m_hWnd);
		if(GetActiveView())
		((CLRptDesignerView*)(GetActiveView()))->OnEnableOwnerDrawCtrl(TRUE);
	}	
}
void CLRptDesignerApp::OnFileOpen()
{
	CWinApp::OnFileOpen();
}
*/
