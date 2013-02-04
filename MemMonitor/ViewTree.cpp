
#include "stdafx.h"
#include "ViewTree.h"
#include "MainFrm.h"


#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, &CViewTree::OnTvnSelchanging)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree 메시지 처리기

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}


//------------------------------------------------------------------------
// 선택된 노드가 바뀔때 마다 호출됨
//------------------------------------------------------------------------
void CViewTree::OnTvnSelchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CString str = GetItemText(
		GetSymbolTreeItem(pNMTreeView->itemNew.hItem));
	CString strOld = GetItemText(
		GetSymbolTreeItem(pNMTreeView->itemOld.hItem)); // 디버깅용

	CMainFrame *pFrm = (CMainFrame*)::AfxGetMainWnd();
	pFrm->GetPropertyWnd().UpdateProperty( str );
	// 탭이 하나 이상이라면, MainProperty가 화면에 나오게한다.
	pFrm->GetPropertyWnd().ShowPane(TRUE, TRUE, TRUE);

	*pResult = 0;
}


//------------------------------------------------------------------------
// 메모리 트리에서 선택된 hItem의 메모리 심볼정보 스트링을
// 가진 Tree 노드를 리턴한다.
//------------------------------------------------------------------------
HTREEITEM CViewTree::GetSymbolTreeItem(HTREEITEM hItem)
{
	HTREEITEM hTmp = hItem;
	while (hTmp)
	{
		HTREEITEM hParent = GetParentItem(hTmp );
		if (!hParent)
			break;
		else
			hTmp  = hParent;
	}
	return hTmp;
}


//------------------------------------------------------------------------
// 선택된 노드의 Symbol Tree Item 을 리턴한다.
//------------------------------------------------------------------------
HTREEITEM CViewTree::GetSelectSymbolTreeItem()
{
	return GetSymbolTreeItem( GetSelectedItem() );	
}
