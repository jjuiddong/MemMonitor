//------------------------------------------------------------------------
// Name:    DiaWrapper.h
// Author:  jjuiddong
// Date:    1/10/2013
// 
// Dia SDK 래퍼클래스
//------------------------------------------------------------------------
#ifndef __DIAWRAPPER_H__
#define __DIAWRAPPER_H__

#include <cvconst.h>
#include "dia2.h"

struct IDiaDataSource;
struct IDiaSession;
struct IDiaSymbol;

namespace dia
{
	// Wrapper class
	class CDiaWrapper : public common::CSingleton<CDiaWrapper>
	{
	public:
		CDiaWrapper();
		virtual ~CDiaWrapper();

	protected:
		IDiaDataSource	*m_pDiaDataSource;
		IDiaSession		*m_pDiaSession;
		IDiaSymbol		*m_pGlobalSymbol;
		DWORD			m_dwMachineType;

	public:
		bool						Init(const std::string &pdbFileName);
		IDiaSymbol*		FindType(const std::string &typeName);

	protected:
		ULONGLONG		GetSymbolLength(IDiaSymbol *pSymbol);
	};

	// dia Global Functions
	IDiaSymbol*		FindChildSymbol( const std::string &symbolName, IDiaSymbol *pSymbol, 
															OUT LONG *pOffset=NULL );

	enum SymbolState { NEW_SYMBOL, PARAM_SYMBOL };
	IDiaSymbol*		GetBaseTypeSymbol( IDiaSymbol *pSymbol, OUT SymbolState &result  );

	std::string			GetSymbolName(IDiaSymbol *pSymbol);
	LONG					GetSymbolLocation(IDiaSymbol *pSymbol, OUT LocationType *pLocType=NULL);
	std::string			GetSymbolTypeName(IDiaSymbol *pSymbol, bool addOptionName=true);
	std::string			GetBasicTypeName(BasicType btype, ULONGLONG length);
	_variant_t			GetValueFromAddress(void *srcPtr, const BasicType btype, const ULONGLONG length );
	_variant_t			GetValueFromSymbol(void *srcPtr,  IDiaSymbol *pSymbol);
	_variant_t			GetValue(void *srctPtr, VARTYPE varType);
	void						SetValue(void *destPtr, _variant_t value);

}

#endif // __DIAWRAPPER_H__
