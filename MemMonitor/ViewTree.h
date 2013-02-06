
#pragma once

class CViewTree : public CTreeCtrl
{
public:
	CViewTree();
	virtual ~CViewTree();

protected:
	enum State { 
		VIEW,			// 화면에 보여지고 있는 상태.
		REFRESH, // Refresh 상태
	};
	State m_State;	

public:
	void				UpdateMemoryTree();
	HTREEITEM	GetSymbolTreeItem(HTREEITEM hItem);
	HTREEITEM GetSelectSymbolTreeItem();
	void				SelectSymbolItem( CString symbolName );

protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchanging(NMHDR *pNMHDR, LRESULT *pResult);
};
