//------------------------------------------------------------------------
// Name:    VisualizerPropertyMaker.h
// Author:  jjuiddong
// Date:    1/29/2013
// 
// 
//------------------------------------------------------------------------
#ifndef __VISUALIZERPROPERTYMAKER_H__
#define __VISUALIZERPROPERTYMAKER_H__

#include "VisualizerDefine.h"

struct IDiaSymbol;
class CPropertiesWnd;
namespace visualizer
{
	 // init
	bool OpenVisualizerScript( const std::string &fileName );
	void Release();

	// make property
	bool	MakeVisualizerProperty( CPropertiesWnd *pPropertiesWnd, 
		CMFCPropertyGridProperty *pParentProp, 
		const sharedmemory::SMemoryInfo &memInfo, const std::string &symbolName );

}

#endif //  __VISUALIZERPROPERTYMAKER_H__
