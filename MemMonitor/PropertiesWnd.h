
#pragma once

#include "Lib/DiaWrapper.h"
#include "DataProperty.h"

class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CPropertiesWnd : public CDockablePane
{
public:
	CPropertiesWnd();
	virtual ~CPropertiesWnd();

protected:
	enum 
	{
		REFRESH_TIME = 1000,
		ID_COMBOBOX = 100,
		ID_TIMER,
	};

	enum State {
		VIEW,
		REFRESH,
	};

	State							m_State;
	CFont							m_fntPropList;
	CPropertiesToolBar		m_wndToolBar;
	CComboBox					m_wndObjectCombo;
	CDataProperty				m_wndPropList;
	CString							m_CurrentSymbolName;

public:
	void	UpdateProperty(const CString &symbolName);
	void	AdjustLayout();
	void	SetVSDotNetLook(BOOL bSet);
	void	UpdateComboBox();

protected:
	void		InitPropList();
	void		ChangeComboBoxFocus(const CString &symbolName);
	void		SetPropListFont();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnRefresh();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnComboBoxSelChange(UINT nID);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

