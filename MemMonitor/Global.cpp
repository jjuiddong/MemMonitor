
#include "stdafx.h"
#include "Global.h"
#include "MainFrm.h"
#include "OutputWnd.h"

using namespace  global;
using namespace std;


//------------------------------------------------------------------------
// OutputWnd 창에 출력
//------------------------------------------------------------------------
void global::PrintOutputWnd( const std::string &str )
{
	CMainFrame *pFrm = (CMainFrame*)::AfxGetMainWnd();
	pFrm->GetOutputWnd().AddString( 
		common::string2wstring(str).c_str() );
}

