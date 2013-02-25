
#pragma once

#include "ViewTree.h"

class CClassToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CMemoryView : public CDockablePane
{
public:
	CMemoryView();
	virtual ~CMemoryView();

protected:
	enum 
	{
		REFRESH_TIME = 60000, // 1분에 한번씩 업데이트
		ID_TIMER,
	};

	CClassToolBar m_wndToolBar;
	CViewTree m_wndClassView;
	CImageList m_ClassViewImages;
	UINT m_nCurrSort;

public:
	void UpdateMemoryView();
	void AdjustLayout();
	void OnChangeVisualStyle();

protected:
	HTREEITEM GetSymbolTreeItem(HTREEITEM hItem);
	void				Refresh();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);
	afx_msg void OnMemoryOpenWindow();
	afx_msg void OnButtonRefresh();
	afx_msg void OnButtonOpenVisualizer();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

	// 	afx_msg void OnClassAddMemberFunction();
	// 	afx_msg void OnClassAddMemberVariable();
	// 	afx_msg void OnClassDefinition();
	// 	afx_msg void OnClassProperties();
	//	afx_msg void OnNewFolder();
	//	afx_msg void OnSort(UINT id);
	//	afx_msg void OnUpdateSort(CCmdUI* pCmdUI);

public:
	afx_msg void OnDestroy();
};

