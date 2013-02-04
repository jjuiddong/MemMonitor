
// MemMonitorView.cpp : CMemMonitorView 클래스의 구현
//

#include "stdafx.h"
#include "MemMonitor.h"
#include "MemMonitorDoc.h"
#include "MemMonitorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMemMonitorView

IMPLEMENT_DYNCREATE(CMemMonitorView, CView)

BEGIN_MESSAGE_MAP(CMemMonitorView, CView)
END_MESSAGE_MAP()

// CMemMonitorView 생성/소멸

CMemMonitorView::CMemMonitorView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CMemMonitorView::~CMemMonitorView()
{
}

BOOL CMemMonitorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	return CView::PreCreateWindow(cs);
}

// CMemMonitorView 그리기

void CMemMonitorView::OnDraw(CDC* /*pDC*/)
{
	CMemMonitorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


void CMemMonitorView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMemMonitorView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CMemMonitorView 진단

#ifdef _DEBUG
void CMemMonitorView::AssertValid() const
{
	CView::AssertValid();
}

void CMemMonitorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMemMonitorDoc* CMemMonitorView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMemMonitorDoc)));
	return (CMemMonitorDoc*)m_pDocument;
}
#endif //_DEBUG


// CMemMonitorView 메시지 처리기
