
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CViewTree 창입니다.

class CViewTree : public CTreeCtrl
{
// 생성입니다.
public:
	CViewTree();
	virtual ~CViewTree();

// 재정의입니다.
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// 구현입니다.
public:
	HTREEITEM GetSymbolTreeItem(HTREEITEM hItem);
	HTREEITEM GetSelectSymbolTreeItem();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchanging(NMHDR *pNMHDR, LRESULT *pResult);
};
