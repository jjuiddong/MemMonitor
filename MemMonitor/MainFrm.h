
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once
#include "FileView.h"
#include "ClassView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"

class CMainFrame : public CFrameWndEx
{
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, 
		CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CMFCMenuBar				m_wndMenuBar;
//	CMFCToolBar				m_wndToolBar;
//	CMFCStatusBar			m_wndStatusBar;
	CMFCToolBarImages	m_UserImages;
//	CFileView						m_wndFileView;
	CClassView					m_wndClassView;
	COutputWnd				m_wndOutput;
	CPropertiesWnd			m_wndProperties;		// main properties

	typedef std::list<CPropertiesWnd*>	PropertyList;
	typedef PropertyList::iterator PropertyItor;
	PropertyList					m_PropertyList;

public:
	bool							OpenScript( const std::string &openScriptFileName );

	bool							AddPropertyWnd( const CString &symbolTypeName,
																CRect rect=CRect(0, 0, 200, 200));

	CPropertiesWnd&		GetPropertyWnd() { return m_wndProperties; }
	CClassView&			GetClassView() { return m_wndClassView; }
	COutputWnd&			GetOutputWnd() { return m_wndOutput; }	

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMemorytreeWindow();
	afx_msg void OnOutputWindow();
	afx_msg void OnPropertyWindow();
	afx_msg void OnClose();
};
