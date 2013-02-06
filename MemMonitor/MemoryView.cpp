
#include "stdafx.h"
#include "MemMonitor.h"
#include "MemoryView.h"
#include "MainFrm.h"
#include "visualizer/PropertyMaker.h"


class CClassViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CMemoryView;

	DECLARE_SERIAL(CClassViewMenuButton)

public:
	CClassViewMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CClassViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// 생성/소멸
//////////////////////////////////////////////////////////////////////

CMemoryView::CMemoryView()
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
}

CMemoryView::~CMemoryView()
{
}

BEGIN_MESSAGE_MAP(CMemoryView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
// 	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
// 	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
// 	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
// 	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
//	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
//	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
//	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)
	ON_COMMAND(ID_MEMORY_OPENWINDOW, &CMemoryView::OnMemoryOpenWindow)
	ON_COMMAND(ID_BUTTON_REFRESH, &CMemoryView::OnButtonRefresh )
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassView 메시지 처리기

int CMemoryView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_FULLROWSELECT
		| TVS_SHOWSELALWAYS  
		| TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndClassView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("클래스 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	// 이미지를 로드합니다.
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_wndToolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE );
 
 	OnChangeVisualStyle();

 	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
 	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & 
		~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM 
		| CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
 
	m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

// 	CMenu menuSort;
// 	menuSort.LoadMenu(IDR_POPUP_SORT);
// 	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CClassViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));
// 	CClassViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));
// 	if (pButton != NULL)
// 	{
// 		pButton->m_bText = FALSE;
// 		pButton->m_bImage = TRUE;
// 		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
// 		pButton->SetMessageWnd(this);
// 	}

	// 정적 트리 뷰 데이터를 더미 코드로 채웁니다.
	//void InitMemoryView();();

	// 타이머 설정
	SetTimer(ID_TIMER, REFRESH_TIME, NULL);

	return 0;
}

void CMemoryView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CMemoryView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndClassView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 클릭한 항목을 선택합니다.
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	CMenu menu;
	menu.LoadMenu(IDR_POPUP_MEMORY);
	CMenu* pSumMenu = menu.GetSubMenu(0);
	theApp.GetContextMenuManager()->ShowPopupMenu((HMENU)pSumMenu->m_hMenu, point.x, point.y, this, TRUE);
}

void CMemoryView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

 	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
 
 	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
 	m_wndClassView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CMemoryView::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	CRect rectTree;
	m_wndClassView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CMemoryView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndClassView.SetFocus();
}

void CMemoryView::OnChangeVisualStyle()
{
	m_ClassViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

// 	m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
// 	m_ClassViewImages.Add(&bmp, RGB(255, 0, 0));
// 
// 	m_wndClassView.SetImageList(&m_ClassViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* 잠금 */);
}


//------------------------------------------------------------------------
// Clicked Open Window Menu
//------------------------------------------------------------------------
void CMemoryView::OnMemoryOpenWindow()
{
	HTREEITEM hItem = m_wndClassView.GetSelectSymbolTreeItem();
	CString itemName = m_wndClassView.GetItemText(hItem);
	CMainFrame *pFrm = (CMainFrame*)::AfxGetMainWnd();	
	pFrm->AddPropertyWnd(itemName);
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CMemoryView::UpdateMemoryView()
{
	global::PrintOutputWnd( "Update MemoryView" );
	m_wndClassView.UpdateMemoryTree();
}


//------------------------------------------------------------------------
// Clicked Refresh Button
//------------------------------------------------------------------------
void CMemoryView::OnButtonRefresh()
{
	global::PrintOutputWnd( "MemoryView Refresh" );
	Refresh();
	
	// visualizer reload
	visualizer::OpenVisualizerScript( "autoexp.txt" );
}


//------------------------------------------------------------------------
//  Refresh
//------------------------------------------------------------------------
void	CMemoryView::Refresh()
{
	UpdateMemoryView();
	CMainFrame *pFrm = (CMainFrame*)::AfxGetMainWnd();
	pFrm->RefreshPropertyWndComboBox();
}

//------------------------------------------------------------------------
// 일정시간마다 정보를 업데이트 한다.
//------------------------------------------------------------------------
void CMemoryView::OnTimer(UINT_PTR nIDEvent)
{
	if (ID_TIMER == nIDEvent)
	{
		Refresh();
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CMemoryView::OnDestroy()
{
	CDockablePane::OnDestroy();
	KillTimer(ID_TIMER);
}
