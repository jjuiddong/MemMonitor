//------------------------------------------------------------------------
// Name:    DefaultPropertyMaker.h
// Author:  jjuiddong
// Date:    1/30/2013
// 
// 
//------------------------------------------------------------------------
#ifndef __DEFAULTPROPERTYMAKER_H__
#define __DEFAULTPROPERTYMAKER_H__

#include "VisualizerGlobal.h"

class CDataProperty;
namespace visualizer
{
	bool		MakeProperty_DefaultForm( CDataProperty *pProperties,  const std::string &symbolName );

	bool		MakeProperty_DefaultForm(  CDataProperty *pProperties, CMFCPropertyGridProperty *pParentProp,  
		const std::string &symbolName );

	bool		MakeProperty_DefaultForm(  CDataProperty *pProperties, CMFCPropertyGridProperty *pParentProp,  
		const SSymbolInfo &symbol );

	bool		MakePropertyChild_DefaultForm(  CDataProperty *pProperties, CMFCPropertyGridProperty *pParentProp,  
		const SSymbolInfo &symbol );

	bool		SetPropertyValue(CMFCPropertyGridProperty *pProp, _variant_t value);

}

#endif // __DEFAULTPROPERTYMAKER_H__
