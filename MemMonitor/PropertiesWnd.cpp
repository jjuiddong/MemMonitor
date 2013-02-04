
#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "MemMonitor.h"
#include "Lib/DiaWrapper.h"
#include "dia2.h"
#include <boost/interprocess/streams/bufferstream.hpp>
#include "visualizer/PropertyMaker.h"
#include "visualizer/DefaultPropertyMaker.h"


using namespace dia;
using namespace sharedmemory;
using namespace boost::interprocess;


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd() :
	m_CurrentSymbolName(L"None")
{
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_CONTROL_RANGE(CBN_SELCHANGE, ID_COMBOBOX, ID_COMBOBOX, OnComboBoxSelChange)
	ON_WM_TIMER()

	//	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	//	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	// 	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	// 	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	// 	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	// 	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	// 	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)

	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 메시지 처리기

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	m_wndObjectCombo.GetWindowRect(&rectCombo);

	int cyCmb = rectCombo.Size().cy;
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), 200, 
		SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), cyTlb, 
		SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), 
		rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER 
													 | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, ID_COMBOBOX))
	{
		TRACE0("속성 콤보 상자를 만들지 못했습니다. \n");
		return -1;
	}
	m_wndObjectCombo.SetFont(CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT)));
	InitComboBox();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("속성 표를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}
	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// 타이머 설정
	SetTimer(ID_TIMER, REFRESH_TIME, NULL);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

// void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* pCmdUI)
// {
// }

// void CPropertiesWnd::OnSortProperties()
// {
// 	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
// }
// 
// void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
// {
// 	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
// }
//
// void CPropertiesWnd::OnProperties1()
// {
// 	// TODO: 여기에 명령 처리기 코드를 추가합니다.
// }
// 
// void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
// {
// 	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
// }
// 
// void CPropertiesWnd::OnProperties2()
// {
// 	// TODO: 여기에 명령 처리기 코드를 추가합니다.
// }
// 
// void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
// {
// 	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
// }

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

/*
	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("모양"));

	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D 모양"), (_variant_t) false, _T("창에 굵은 글꼴이 아닌 글꼴을 지정하고, 컨트롤에 3D 테두리를 지정합니다.")));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("테두리"), _T("Dialog Frame"), _T("None, Thin, Resizable 또는 Dialog Frame 중 하나를 지정합니다."));
	pProp->AddOption(_T("None"));
	pProp->AddOption(_T("Thin"));
	pProp->AddOption(_T("Resizable"));
	pProp->AddOption(_T("Dialog Frame"));
	pProp->AllowEdit(FALSE);

	pGroup1->AddSubItem(pProp);
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("캡션"), (_variant_t) _T("정보"), _T("창의 제목 표시줄에 표시되는 텍스트를 지정합니다.")));

	m_wndPropList.AddProperty(pGroup1);

	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("창 크기"), 0, TRUE);

	pProp = new CMFCPropertyGridProperty(_T("높이"), (_variant_t) 250l, _T("창의 높이를 지정합니다."));
	pProp->EnableSpinControl(TRUE, 50, 300);
	pSize->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty( _T("너비"), (_variant_t) 150l, _T("창의 너비를 지정합니다."));
	pProp->EnableSpinControl(TRUE, 50, 200);
	pSize->AddSubItem(pProp);

	m_wndPropList.AddProperty(pSize);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("글꼴"));

	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);

	lstrcpy(lf.lfFaceName, _T("맑은 고딕"));

	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("글꼴"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("창의 기본 글꼴을 지정합니다.")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("시스템 글꼴을 사용합니다."), (_variant_t) true, _T("창에서 MS Shell Dlg 글꼴을 사용하도록 지정합니다.")));

	m_wndPropList.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("기타"));
	pProp = new CMFCPropertyGridProperty(_T("(이름)"), _T("응용 프로그램"));
	pProp->Enable(FALSE);
	pGroup3->AddSubItem(pProp);

	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("창 색상"), RGB(210, 192, 254), NULL, _T("창의 기본 색상을 지정합니다."));
	pColorProp->EnableOtherButton(_T("기타..."));
	pColorProp->EnableAutomaticButton(_T("기본값"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);

	static TCHAR BASED_CODE szFilter[] = _T("아이콘 파일(*.ico)|*.ico|모든 파일(*.*)|*.*||");
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("아이콘"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("창 아이콘을 지정합니다.")));

	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("폴더"), _T("c:\\")));

	m_wndPropList.AddProperty(pGroup3);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("계층"));

	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("첫번째 하위 수준"));
	pGroup4->AddSubItem(pGroup41);

	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("두 번째 하위 수준"));
	pGroup41->AddSubItem(pGroup411);

	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("항목 1"), (_variant_t) _T("값 1"), _T("설명입니다.")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("항목 2"), (_variant_t) _T("값 2"), _T("설명입니다.")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("항목 3"), (_variant_t) _T("값 3"), _T("설명입니다.")));

	pGroup4->Expand(FALSE);
	m_wndPropList.AddProperty(pGroup4);
/**/
}


//------------------------------------------------------------------------
// 콤보박스 초기화
// 공유메모리에 등록된 포인터를 콤보박스에 추가한다.
//------------------------------------------------------------------------
void	CPropertiesWnd::InitComboBox()
{
	sharedmemory::MemoryList memList;
	sharedmemory::EnumerateMemoryInfo(memList);

	m_wndObjectCombo.AddString( L"None" ); // default
	BOOST_FOREACH(sharedmemory::SMemoryInfo &info, memList)
	{
		std::wstring wstr = common::string2wstring( info.name );
		m_wndObjectCombo.AddString( wstr.c_str() );
	}	

	m_wndObjectCombo.SetCurSel(0);
}


//------------------------------------------------------------------------
// symbolName 의 이름과 동일한 콤보박스 스트링을 
// 화면에 보이게 한다.
//------------------------------------------------------------------------
void	CPropertiesWnd::ChangeComboBoxFocus(const CString &symbolName)
{
	const int nItem = m_wndObjectCombo.FindString(0, symbolName);
	if (nItem == CB_ERR)
	{
		m_wndObjectCombo.SetCurSel(0);
		AfxMessageBox( common::formatw("\"%s\" Item을 찾지 못했습니다.",
			(LPCWSTR)symbolName).c_str() );
		return;
	}
	m_wndObjectCombo.SetCurSel( nItem );
}


void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}

void CPropertiesWnd::SetVSDotNetLook(BOOL bSet)
{
	m_wndPropList.SetVSDotNetLook(bSet);
	m_wndPropList.SetGroupNameFullWidth(bSet);
}


//------------------------------------------------------------------------
// 인자로 넘어온 symbolType대로 Property를 생성한다.
//------------------------------------------------------------------------
void CPropertiesWnd::UpdateProperty(const CString &symbolName)
{
	if (m_CurrentSymbolName == symbolName)
		return;

	m_wndPropList.RemoveAll();
	BOOST_FOREACH(SPropItem *item, m_PropList)
	{
		SAFE_DELETE(item);
	}
	m_PropList.clear();

	std::string tmpStr = common::wstring2string((LPCWSTR)symbolName);
	std::string str = sharedmemory::ParseObjectName(tmpStr);

	visualizer::MakeProperty_DefaultForm(this, tmpStr);

	IDiaSymbol *pSymbol = CDiaWrapper::Get()->FindType(str);
	if (!pSymbol)
	{
		::AfxMessageBox(
			common::formatw("\"%s\" 해당하는 심볼은 Pdb파일에 없습니다.", 
				tmpStr.c_str()).c_str() );
		SetWindowTextW(L"Not Found");
		return;
	}

	sharedmemory::SMemoryInfo memInfo;
	if (!sharedmemory::FindMemoryInfo(tmpStr, memInfo))
	{
		::AfxMessageBox( 
				common::formatw("공유메모리에 %s 타입의 정보가 없습니다.",
					tmpStr.c_str()).c_str() );
		SetWindowTextW(L"Not Found");
		pSymbol->Release();
		return;
	}

	// Output창에 출력
	CMainFrame *pFrm = (CMainFrame*)::AfxGetMainWnd();
	CString output = L"[ " + symbolName;
	output += L" ] 심볼 변경";
	pFrm->GetOutputWnd().AddString( output );
	dbg::Print(  "%s 심볼 변경", tmpStr.c_str() );
	//

	SetWindowTextW(symbolName);
	m_CurrentSymbolName = symbolName;
	ChangeComboBoxFocus(symbolName);

	if (m_wndPropList.GetPropertyCount() > 0)
	{
		CMFCPropertyGridProperty *prop = m_wndPropList.GetProperty(0);
		prop->Expand();
	}
	pSymbol->Release();
}


//------------------------------------------------------------------------
// Property추가
//------------------------------------------------------------------------
void CPropertiesWnd::AddProperty(
								 CMFCPropertyGridProperty *pParentProp, 
								 CMFCPropertyGridProperty *prop, 
								 STypeData *pTypeData)
{
	RET(!prop);

	SPropItem *p = new SPropItem;
	p->prop = prop;
	p->typeData = *pTypeData;
	m_PropList.push_back(p);

	prop->SetData((DWORD_PTR)p);

	if (pParentProp)
	{
		pParentProp->AddSubItem(prop);
	}
	else
	{
		m_wndPropList.AddProperty(prop);
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
LRESULT CPropertiesWnd::OnPropertyChanged (WPARAM,LPARAM lParam)
{
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*) lParam;
	SPropItem *pItem = (SPropItem*)pProp->GetData();
	if (pItem && pItem->typeData.ptr)
	{
		_variant_t curVar = pProp->GetValue();
		_variant_t var;
		if (pProp->GetOptionCount() > 0)
		{
			CString str = (LPCTSTR) (_bstr_t)curVar;
			const int idx = FindOption(pProp, str);
			ASSERT_RETV( idx >= 0, 0 );
			_variant_t tmp = idx;
			var.ChangeType(VT_INT, &tmp);
		}
		else
		{
 			var.ChangeType(pItem->typeData.vt, &curVar);
		}

		// 메모리에 저장
		dia::SetValue( pItem->typeData.ptr, var);
	}

	return 0;
}


//------------------------------------------------------------------------
// ComboBox 아이템 위치가 바뀌면 호출된다.
//------------------------------------------------------------------------
void CPropertiesWnd::OnComboBoxSelChange(UINT nID)
{
	const int nItem = m_wndObjectCombo.GetCurSel();
	CString curText;
	m_wndObjectCombo.GetLBText(nItem , curText);
	UpdateProperty( curText );
}


//------------------------------------------------------------------------
// Clicked Refresh Button
//------------------------------------------------------------------------
void CPropertiesWnd::OnRefresh()
{
	Refresh();

	// Output창에 출력
	CMainFrame *pFrm = (CMainFrame*)::AfxGetMainWnd();
	pFrm->GetOutputWnd().AddString( L"Refresh" );
	//	
}


//------------------------------------------------------------------------
// 공유메모리 정보를 갱신한다.
//------------------------------------------------------------------------
void CPropertiesWnd::Refresh()
{
	const int cnt = m_wndPropList.GetPropertyCount();
	for (int i=0; i < cnt; ++i)
	{
		CMFCPropertyGridProperty *pProp = m_wndPropList.GetProperty(i);
		Refresh_Property(pProp);
	}
}


//------------------------------------------------------------------------
// 공유메모리 정보를 갱신한다.
//------------------------------------------------------------------------
void	CPropertiesWnd::Refresh_Property(CMFCPropertyGridProperty *pProp )
{
	RET(!pProp);

	if (pProp->IsInPlaceEditing()) // Edit중인 값은 갱신하지 않는다.
		return;

	SPropItem *pItem = (SPropItem*)pProp->GetData();
	if (pItem && 
		(SymTagData == pItem->typeData.symtag 
		|| SymTagBaseType == pItem->typeData.symtag 
		||  SymTagEnum == pItem->typeData.symtag 
		)
		&& pItem->typeData.ptr)
	{
		_variant_t value = dia::GetValue(pItem->typeData.ptr, pItem->typeData.vt);
		visualizer::SetPropertyValue(pProp, value);
	}

	const int subCnt = pProp->GetSubItemsCount();
	for (int k=0; k < subCnt; ++k)
	{
		CMFCPropertyGridProperty *p = pProp->GetSubItem(k);
		Refresh_Property(p);
	}
}


//------------------------------------------------------------------------
//  일정 시간마다 정보를 갱신한다.
//------------------------------------------------------------------------
void CPropertiesWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (ID_TIMER == nIDEvent)
	{
		// 화면에 보이지 않거나, 드래그 중일때는 리프레쉬하지 않는다.
		if (IsDragMode() || !CWnd::IsWindowVisible())
			return;
		Refresh();
	}
	CDockablePane::OnTimer(nIDEvent);
}


//------------------------------------------------------------------------
// pProp의 Option중에 findVar 와 같은 데이타를 찾는다.
// 없다면 -1을 리턴한다.
// findVar 는 스트링을 가르키는 데이터여야 한다.
//------------------------------------------------------------------------
int CPropertiesWnd::FindOption( CMFCPropertyGridProperty *pProp, const CString &findStr )
{
	RETV(!pProp, -1);

	const int cnt = pProp->GetOptionCount();
	for (int i=0; i < cnt; ++i)
	{
		CString str = (LPCTSTR) (_bstr_t)pProp->GetOption(i);
		if (str == findStr)
			return i;		
	}
	return -1;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CPropertiesWnd::OnDestroy()
{
	CDockablePane::OnDestroy();

	KillTimer(ID_TIMER);
	BOOST_FOREACH(SPropItem *item, m_PropList)
	{
		SAFE_DELETE(item);
	}
	m_PropList.clear();
}
