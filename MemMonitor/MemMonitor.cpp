
// MemMonitor.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "MemMonitor.h"
#include "MainFrm.h"

#include "MemMonitorDoc.h"
#include "MemMonitorView.h"
#include "Lib/DiaWrapper.h"
#include <typeinfo.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "visualizer/PropertyMaker.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::string g_configFileName("config.json");

// CMemMonitorApp

BEGIN_MESSAGE_MAP(CMemMonitorApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CMemMonitorApp::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 표준 인쇄 설정 명령입니다.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CMemMonitorApp 생성

CMemMonitorApp::CMemMonitorApp()
{

	m_bHiColorIcons = TRUE;

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

CMemMonitorApp::~CMemMonitorApp()
{
	dia::CDiaWrapper::Get()->Release();
	sharedmemory::Release();
	visualizer::Release();
}


// 유일한 CMemMonitorApp 개체입니다.

CMemMonitorApp theApp;


// CMemMonitorApp 초기화

BOOL CMemMonitorApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 표준 초기화	
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.
// 	InitContextMenuManager();
// 	InitKeyboardManager();
// 	InitTooltipManager();
// 	CMFCToolTipInfo ttParams;
// 	ttParams.m_bVislManagerTheme = TRUE;
// 	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
// 		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);
	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if (!cmdInfo.m_strFileName.IsEmpty())
		g_configFileName = common::wstr2str((LPCTSTR)cmdInfo.m_strFileName);

	if (!OpenConfigScript(g_configFileName))
		return FALSE;

	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMemMonitorDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CMemMonitorView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.

	visualizer::OpenVisualizerScript( "autoexp.txt" );
	((CMainFrame*)AfxGetMainWnd())->OpenScript(  g_configFileName );

	return TRUE;
}


//------------------------------------------------------------------------
// Config 스크립트를 열어서 기본환경을 갖춘다.
// pdb  파일을 열고, shared memory 에 접근한다.
//------------------------------------------------------------------------
BOOL CMemMonitorApp::OpenConfigScript(const std::string &configFileName)
{
	try
	{
		// boost property tree
		using boost::property_tree::ptree;
		using std::string;
		ptree props;
		boost::property_tree::read_json(configFileName.c_str(), props);
		string pdbPath = props.get<string>("pdbpath");
		string shareMemoryName = props.get<string>("sharedmemoryname");

		// Pdb Load
		if (!dia::CDiaWrapper::Get()->Init(pdbPath))
		{
			CString msg;
			msg.Format( L"%s Pdb 파일이 없습니다.", 
				common::str2wstr(pdbPath).c_str() );
			::AfxMessageBox( msg );
			return FALSE;
		}
		if (!sharedmemory::Init(shareMemoryName, sharedmemory::SHARED_CLIENT))
		{
			CString msg;
			msg.Format( L"%s  이름의 공유메모리가 없습니다.", 
				common::str2wstr(shareMemoryName).c_str() );
			::AfxMessageBox( msg );
			return FALSE;
		}
	}
	catch (std::exception &e)
	{
		CString msg = common::formatw("\"%s\" json script Err!! [%s]",  
			configFileName.c_str(), e.what()).c_str();
		::AfxMessageBox( msg );
		return FALSE;
	}
	return TRUE;
}


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CMemMonitorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMemMonitorApp 사용자 지정 로드/저장 메서드

void CMemMonitorApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CMemMonitorApp::LoadCustomState()
{
}

void CMemMonitorApp::SaveCustomState()
{
}

// CMemMonitorApp 메시지 처리기



