//------------------------------------------------------------------------
// Name:    DefaultPropertyMaker.h
// Author:  jjuiddong
// Date:    1/30/2013
// 
// 
//------------------------------------------------------------------------
#ifndef __DEFAULTPROPERTYMAKER_H__
#define __DEFAULTPROPERTYMAKER_H__

#include "VisualizerDefine.h"

struct IDiaSymbol;
class CPropertiesWnd;
struct STypeData;
namespace visualizer
{
	bool		MakeProperty_DefaultForm( CPropertiesWnd *pProperties,  const std::string &symbolName );

	bool		MakeProperty_DefaultForm(  CPropertiesWnd *pProperties, CMFCPropertyGridProperty *pParentProp,  
		const std::string &symbolName );

	bool		MakeProperty_DefaultForm(  CPropertiesWnd *pProperties, CMFCPropertyGridProperty *pParentProp,  
		IDiaSymbol *pSymbol, const sharedmemory::SMemoryInfo &memInfo );

	bool		SetPropertyValue(CMFCPropertyGridProperty *pProp, _variant_t value);

}

#endif // __DEFAULTPROPERTYMAKER_H__
