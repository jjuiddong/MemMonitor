//------------------------------------------------------------------------
// Name:    Global.h
// Author:  jjuiddong
// Date:    2/4/2013
// 
// 전역함수 모음
//------------------------------------------------------------------------
#ifndef __GLOBAL_H__
#define  __GLOBAL_H__

#include <cvconst.h>

namespace global
{
	void PrintOutputWnd( const std::string &str );

}

struct STypeData
{
	enum SymTagEnum symtag;
	VARTYPE vt;
	void *ptr;
	STypeData( enum SymTagEnum _symtag,  VARTYPE _vt, void *_ptr) : 
	symtag(_symtag), vt(_vt), ptr(_ptr) { }
	STypeData() {}
};


#endif // __GLOBAL_H__
