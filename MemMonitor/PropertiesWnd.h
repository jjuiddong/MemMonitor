
#pragma once

#include "Lib/DiaWrapper.h"

class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

struct STypeData
{
	enum SymTagEnum symtag;
	VARTYPE vt;
	void *ptr;
	STypeData( enum SymTagEnum _symtag,  VARTYPE _vt, void *_ptr) : 
	symtag(_symtag), vt(_vt), ptr(_ptr) { }
	STypeData() {}
};


class CPropertiesWnd : public CDockablePane
{
// 생성입니다.
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

	typedef struct _SPropItem
	{
		CMFCPropertyGridProperty *prop;
		STypeData typeData;

	} SPropItem;
	typedef std::list<SPropItem*> PropList;
	typedef PropList::iterator PropItor;

	PropList						m_PropList;
	CFont							m_fntPropList;
	CPropertiesToolBar		m_wndToolBar;
	CComboBox					m_wndObjectCombo;
	CMFCPropertyGridCtrl	m_wndPropList;
	CString							m_CurrentSymbolName;

public:
	void	UpdateProperty(const CString &symbolName);
	void	AdjustLayout();
	void	SetVSDotNetLook(BOOL bSet);
	void Refresh();

	void		AddProperty(CMFCPropertyGridProperty *pParentProp, 
		CMFCPropertyGridProperty *prop, STypeData *pTypeData);

protected:
	void		Refresh_Property(CMFCPropertyGridProperty *pProp );

	void		InitPropList();
	void		InitComboBox();
	void		ChangeComboBoxFocus(const CString &symbolName);
	void		SetPropListFont();
	int		FindOption( CMFCPropertyGridProperty *pProp, const CString &findStr );

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnRefresh();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnPropertyChanged (WPARAM,LPARAM);
	afx_msg void OnComboBoxSelChange(UINT nID);

	//		afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	// 	afx_msg void OnSortProperties();
	// 	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	// 	afx_msg void OnProperties1();
	// 	afx_msg void OnUpdateProperties1(CCmdUI* pCmdUI);
	// 	afx_msg void OnProperties2();
	// 	afx_msg void OnUpdateProperties2(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
};

