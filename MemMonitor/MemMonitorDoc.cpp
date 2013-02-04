
// MemMonitorDoc.cpp : CMemMonitorDoc 클래스의 구현
//

#include "stdafx.h"
#include "MemMonitor.h"

#include "MemMonitorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMemMonitorDoc

IMPLEMENT_DYNCREATE(CMemMonitorDoc, CDocument)

BEGIN_MESSAGE_MAP(CMemMonitorDoc, CDocument)
END_MESSAGE_MAP()


// CMemMonitorDoc 생성/소멸

CMemMonitorDoc::CMemMonitorDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CMemMonitorDoc::~CMemMonitorDoc()
{
}

BOOL CMemMonitorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CMemMonitorDoc serialization

void CMemMonitorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CMemMonitorDoc 진단

#ifdef _DEBUG
void CMemMonitorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMemMonitorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMemMonitorDoc 명령
