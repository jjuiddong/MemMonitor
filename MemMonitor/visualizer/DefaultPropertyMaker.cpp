
#include "stdafx.h"
#include "DefaultPropertyMaker.h"
#include "PropertyMaker.h"
#include "../Lib/DiaWrapper.h"
#include "../PropertiesWnd.h"


namespace visualizer
{
	using namespace sharedmemory;
	using namespace visualizer_parser;

	CPropertiesWnd *n_pProperty = NULL;

	// make property
	void		MakeProperty_Root(CMFCPropertyGridProperty *pParentProp, 
		IDiaSymbol *pSymbol, const SMemoryInfo &memInfo);

	void		MakeProperty_UDT(CMFCPropertyGridProperty *pParentProp, 
		IDiaSymbol *pSymbol, const SMemoryInfo &memInfo);

	void		MakeProperty_BaseClass(CMFCPropertyGridProperty *pParentProp, 
		IDiaSymbol *pSymbol, const SMemoryInfo &memInfo);

	void		MakeProperty_Pointer(CMFCPropertyGridProperty *pParentProp, 
		IDiaSymbol *pSymbol, const SMemoryInfo &memInfo);

	void		MakeProperty_Data(CMFCPropertyGridProperty *pParentProp, 
		IDiaSymbol *pSymbol, const SMemoryInfo &memInfo);

	void		MakeProperty_Array(CMFCPropertyGridProperty *pParentProp, 
		IDiaSymbol *pSymbol, const SMemoryInfo &memInfo);

	void		MakeProperty_Enum(CMFCPropertyGridProperty *pParentProp, 
		IDiaSymbol *pSymbol, const SMemoryInfo &memInfo);

	CMFCPropertyGridProperty* MakeProperty_BaseType(
		CMFCPropertyGridProperty *pParentProp, 
		const std::string valueName, IDiaSymbol *pSymbol, 
		const SMemoryInfo &memInfo );

	void		AddProperty(CMFCPropertyGridProperty *pParentProp, 
		CMFCPropertyGridProperty *prop,  STypeData *pTypeData);

	_variant_t GetValue(IDiaSymbol *pSymbol, void *pMemPtr);

}

using namespace dia;
using namespace std;
using namespace visualizer;


//------------------------------------------------------------------------
// 기본형식으로 property창에 심볼을 출력한다.
// symbolName : 공유메모리에 저장된 심볼이름
//------------------------------------------------------------------------
bool visualizer::MakeProperty_DefaultForm( CPropertiesWnd *pProperties,  const string &symbolName )
{
	const std::string str = sharedmemory::ParseObjectName(symbolName);

	IDiaSymbol *pSymbol = CDiaWrapper::Get()->FindType(str);
	RETV(!pSymbol, false);

	sharedmemory::SMemoryInfo memInfo;
	if (!sharedmemory::FindMemoryInfo(symbolName, memInfo))
	{
		pSymbol->Release();
		return false;
	}

	MakeProperty_DefaultForm(pProperties, NULL, pSymbol, memInfo);
	pSymbol->Release();
	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool visualizer::MakeProperty_DefaultForm(  CPropertiesWnd *pProperties, 
								 CMFCPropertyGridProperty *pParentProp,  const string &symbolName )
{

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool visualizer::MakeProperty_DefaultForm(  CPropertiesWnd *pProperties,
				CMFCPropertyGridProperty *pParentProp,  
				 IDiaSymbol *pSymbol, const sharedmemory::SMemoryInfo &memInfo )
{
	n_pProperty = pProperties;
	MakeProperty_Root(pParentProp, pSymbol, memInfo);
	return true;
}


//------------------------------------------------------------------------
// Property 생성
//------------------------------------------------------------------------
void visualizer::MakeProperty_Root(CMFCPropertyGridProperty *pParentProp, 
											IDiaSymbol *pSymbol, const SMemoryInfo &memInfo)
{
	enum SymTagEnum symtag;
	if (S_OK != pSymbol->get_symTag((DWORD*)&symtag))
		return;

	switch (symtag)
	{
	case SymTagData: 
		MakeProperty_Data(pParentProp, pSymbol, memInfo);
		break;

	case SymTagArrayType:
		MakeProperty_Array(pParentProp, pSymbol, memInfo);
		break;

	case SymTagPointerType:
		MakeProperty_Pointer(pParentProp, pSymbol, memInfo);
		break;

	case SymTagBaseClass:
		MakeProperty_BaseClass(pParentProp, pSymbol, memInfo);
		break;

	case SymTagUDT:
		MakeProperty_UDT(pParentProp, pSymbol, memInfo);
		break;

	case SymTagEnum:
		MakeProperty_Enum(pParentProp, pSymbol, memInfo);
		break;

	case SymTagTypedef:
	case SymTagVTable:
	case SymTagFunction:
		break;

	default:
		{
			// 			std::string name = dia::GetSymbolName(pSymbol);
			// 			CMainFrame *pFrm = (CMainFrame*)::AfxGetMainWnd();
			// 			std::stringstream ss;
			// 			ss << name << "값의 데이타를 Property애 추가하지 못했습니다.";
			// 			pFrm->GetOutputWnd().AddString( common::string2wstring(ss.str()).c_str() );
		}
		break;
	}
}


//------------------------------------------------------------------------
// Enum 출력
//------------------------------------------------------------------------
void	visualizer ::MakeProperty_Enum(CMFCPropertyGridProperty *pParentProp, 
													IDiaSymbol *pSymbol, const SMemoryInfo &memInfo)
{
	// 	const std::string name = dia::GetSymbolName(pSymbol);
	// 
	// 	CMFCPropertyGridProperty *pProp = 
	// 		new CMFCPropertyGridProperty( common::string2wstring(name).c_str() ); 
	// 
	// 	pProp->AddOption( L"test1" );
	// 	pProp->AddOption( L"test2" );
	// 	pProp->AddOption( L"test3" );
	// 	pProp->AddOption( L"test4" );


	// 	ULONGLONG length = 0;
	// 	HRESULT hr = pSymbol->get_length(&length);
	// 	assert(S_OK == hr);
	// 
	// 	CComPtr<IDiaEnumSymbols> pEnumChildren;
	// 	IDiaSymbol *pChild;
	// 	ULONG celt = 0;
	// 	if (SUCCEEDED(pSymbol->findChildren(SymTagNull, NULL, nsNone, &pEnumChildren))) 
	// 	{
	// 		while (SUCCEEDED(pEnumChildren->Next(1, &pChild, &celt)) && (celt == 1)) 
	// 		{
	// 			SMemoryInfo udtMemInfo = SMemoryInfo(name.c_str(), memInfo.ptr, (size_t)length);
	// 
	// 			MakeProperty(pProp, pChild, udtMemInfo);
	// 			pChild->Release();
	// 		}
	// 	}
	// 
	// 	pProp->Expand(FALSE); // 일단 접어놓는다.
	//	AddProperty(pParentProp, pProp, STypeData(SymTagUDT, VT_EMPTY, NULL));
}


//------------------------------------------------------------------------
// 기본 클래스 
//------------------------------------------------------------------------
void	visualizer ::MakeProperty_BaseClass(CMFCPropertyGridProperty *pParentProp, 
														 IDiaSymbol *pSymbol, const SMemoryInfo &memInfo)
{
	CComPtr<IDiaSymbol> pBaseType;
	HRESULT hr = pSymbol->get_type(&pBaseType);
	ASSERT_RET(hr == S_OK);

	const std::string name = GetSymbolName(pBaseType);

	ULONGLONG length = 0;
	hr = pBaseType->get_length(&length);
	ASSERT_RET(hr == S_OK);

	LONG offset = 0;
	hr =  pSymbol->get_offset(&offset);
	ASSERT_RET(hr == S_OK);

	BYTE *ptr = (BYTE*)memInfo.ptr + offset;
	SMemoryInfo newMemInfo(name.c_str(), ptr, (size_t)length);
	MakeProperty_Root(pParentProp, pBaseType, newMemInfo);
}


//------------------------------------------------------------------------
// User Define Type 
//------------------------------------------------------------------------
void visualizer ::MakeProperty_UDT(CMFCPropertyGridProperty *pParentProp, 
												IDiaSymbol *pSymbol, const SMemoryInfo &memInfo)
{
	const std::string name = dia::GetSymbolName(pSymbol);

	const bool isVisualizerType = visualizer::MakeVisualizerProperty( n_pProperty, pParentProp, memInfo, name);
	if (isVisualizerType)
	{
		// 아직 하는일 없음 
	}
	else
	{
		CMFCPropertyGridProperty *pProp = 
			new CMFCPropertyGridProperty( common::string2wstring(name).c_str() ); 

		ULONGLONG length = 0;
		HRESULT hr = pSymbol->get_length(&length);
		assert(S_OK == hr);

		CComPtr<IDiaEnumSymbols> pEnumChildren;
		IDiaSymbol *pChild;
		ULONG celt = 0;
		if (SUCCEEDED(pSymbol->findChildren(SymTagNull, NULL, nsNone, &pEnumChildren))) 
		{
			while (SUCCEEDED(pEnumChildren->Next(1, &pChild, &celt)) && (celt == 1)) 
			{
				SMemoryInfo udtMemInfo = SMemoryInfo(name.c_str(), memInfo.ptr, (size_t)length);

				MakeProperty_Root(pProp, pChild, udtMemInfo);
				pChild->Release();
			}
		}

		pProp->Expand(FALSE); // 일단 접어놓는다.
		AddProperty(pParentProp, pProp, &STypeData(SymTagUDT, VT_EMPTY, memInfo.ptr));
	}
}


//------------------------------------------------------------------------
// Pointer 타입 출력 
//------------------------------------------------------------------------
void	visualizer ::MakeProperty_Pointer(CMFCPropertyGridProperty *pParentProp, 
													   IDiaSymbol *pSymbol, const SMemoryInfo &memInfo)
{
	CComPtr<IDiaSymbol> pBaseType;
	HRESULT hr = pSymbol->get_type(&pBaseType);
	ASSERT_RET(hr == S_OK);

	ULONGLONG length = 0;
	hr = pBaseType->get_length(&length);
	ASSERT_RET(hr == S_OK);

	const std::string typeName = dia::GetSymbolTypeName(pSymbol);

	void *srcPtr = (void*)*(DWORD*)memInfo.ptr;
	void *newPtr = sharedmemory::MemoryMapping(srcPtr);

	CMFCPropertyGridProperty *pProp = new CMFCPropertyGridProperty( 
		common::formatw(" %s 0x%x", typeName.c_str(), newPtr).c_str() ); 
	AddProperty( pParentProp, pProp, &STypeData(SymTagPointerType, VT_EMPTY,NULL));

	if (newPtr) 
	{
		SMemoryInfo ptrMemInfo(typeName.c_str(), newPtr, (size_t)length);
		MakeProperty_Root(pProp, pBaseType, ptrMemInfo);
	}
}


//------------------------------------------------------------------------
// Property창에 Control을 추가한다.
// 변수 이름과 타입, 값을 설정한다.
//------------------------------------------------------------------------
void visualizer ::MakeProperty_Data(CMFCPropertyGridProperty *pParentProp, 
												 IDiaSymbol *pSymbol,
												 const SMemoryInfo &memInfo)
{
	CComPtr<IDiaSymbol> pBaseType;
	HRESULT hr = pSymbol->get_type(&pBaseType);
	ASSERT_RET(hr == S_OK);

	enum SymTagEnum baseSymTag;
	hr = pBaseType->get_symTag((DWORD*)&baseSymTag);
	ASSERT_RET(hr == S_OK);

	switch (baseSymTag)
	{
	case SymTagBaseType:
		{
			std::string valueTypeName = dia::GetSymbolName(pSymbol) + " (" + dia::GetSymbolTypeName(pSymbol) + ")";
			MakeProperty_BaseType( pParentProp, valueTypeName, pSymbol, memInfo );
		}
		break;

	case SymTagEnum:
		{
			std::string name = dia::GetSymbolName(pSymbol);
			std::string typeName = dia::GetSymbolTypeName(pBaseType);
			std::string valueTypeName =  name + " (" +  typeName + ")";
			CMFCPropertyGridProperty *pProp = 
				new CMFCPropertyGridProperty( common::string2wstring(valueTypeName).c_str() , _T(" ") ); 

			CComPtr<IDiaEnumSymbols> pEnumChildren;
			IDiaSymbol *pChild;
			if (SUCCEEDED(pBaseType->findChildren(SymTagNull, NULL, nsNone, &pEnumChildren))) 
			{
				ULONG celt = 0;
				while (SUCCEEDED(pEnumChildren->Next(1, &pChild, &celt)) && (celt == 1)) 
				{
					std::string e_name = dia::GetSymbolName(pChild);
					pProp->AddOption( common::string2wstring(e_name).c_str() );
					pChild->Release();
				}
			}
			pProp->AllowEdit(FALSE);

			ULONGLONG offset = dia::GetSymbolLocation(pSymbol);
			ULONGLONG length = 0;
			HRESULT hr = pBaseType->get_length(&length);
			void *ptr = (BYTE*)memInfo.ptr + offset;
			_variant_t value = dia::GetValueFromAddress( ptr, btUInt, length );
			SetPropertyValue( pProp, value);

			AddProperty(pParentProp, pProp, &STypeData(baseSymTag, VT_UI4, ptr));
		}
		break;

	case SymTagPointerType:
	case SymTagUDT:
	case SymTagArrayType:
		{
			std::string name = dia::GetSymbolName(pSymbol);
			std::string typeName = dia::GetSymbolTypeName(pBaseType);
			std::string valueTypeName =  name + " (" +  typeName + ")";
			CMFCPropertyGridProperty *pProp = NULL;

			if (baseSymTag == SymTagPointerType 
				|| baseSymTag == SymTagArrayType )
			{
				CComPtr<IDiaSymbol> pBaseOfBaseType;
				hr = pBaseType->get_type(&pBaseOfBaseType);
				ASSERT_RET(hr == S_OK);

				enum SymTagEnum baseOfBaseSymTag;
				hr = pBaseOfBaseType->get_symTag((DWORD*)&baseOfBaseSymTag);
				ASSERT_RET(hr == S_OK);
				if (SymTagBaseType == baseOfBaseSymTag)
				{
					BasicType btype;
					hr = pBaseOfBaseType->get_baseType((DWORD*)&btype);
					ASSERT_RET(hr == S_OK);

					// char*, char[] 타입이라면 스트링을 출력한다.
					if (btChar == btype)
					{
						void *ptr = NULL;
						std::stringstream ss;
						if (SymTagArrayType == baseSymTag)
						{
							ptr = memInfo.ptr;
							ss << valueTypeName << " {\"" << (char*)ptr << "\"}";
						}
						else if (SymTagPointerType == baseSymTag)
						{
							void *srcPtr = (void*)*(DWORD*)memInfo.ptr;
							ptr = sharedmemory::MemoryMapping(srcPtr);
							if (!ptr)
								ptr = srcPtr; // 공유메모리에 있지 않은 데이타일경우 주소만 출력한다.
							ss << valueTypeName << " 0x" << ptr << " {\"";
							ss << (char*)(sharedmemory::CheckValidAddress(ptr)? ptr : "not shared memory")  << "\"}";
						}

						if (ptr)
						{
							pProp = new CMFCPropertyGridProperty( common::string2wstring(ss.str()).c_str() );
						}
					}
				}
			}

			if (!pProp)
			{
				pProp = new CMFCPropertyGridProperty( common::string2wstring(valueTypeName).c_str() ); 
			}

			AddProperty(pParentProp, pProp, &STypeData(baseSymTag, VT_EMPTY, NULL));

			ULONGLONG offset = dia::GetSymbolLocation(pSymbol);
			ULONGLONG length = 0;
			HRESULT hr = pBaseType->get_length(&length);
			assert(S_OK == hr);	

			void *ptr = (BYTE*)memInfo.ptr + offset;
			SMemoryInfo udtMemInfo = SMemoryInfo(typeName.c_str(), ptr, (size_t)length);
			MakeProperty_Root(pProp, pBaseType, udtMemInfo); // pBaseType을 인자로 한다.			
			pProp->Expand(FALSE);
		}
		break;
	}
}


//------------------------------------------------------------------------
// pSymbol : Array Type을 가리킨다. 
//------------------------------------------------------------------------
void visualizer ::MakeProperty_Array(CMFCPropertyGridProperty *pParentProp, 
												  IDiaSymbol *pSymbol,
												  const sharedmemory::SMemoryInfo &memInfo)
{
	ULONGLONG length=0;
	ULONGLONG element_length=0;
	pSymbol->get_length(&length);

	CComPtr<IDiaSymbol> pElementType;	// 배열 개개의 타입
	if (S_FALSE == pSymbol->get_type(&pElementType))
	{
		// pSymbol은 array타입이기 때문에 basetype을 가져야 한다.
		// 그렇지 않다면 pdb파일이 깨졌거나, 시스템상 오류이다.
		assert(0);
		return;
	}

	HRESULT result = pElementType->get_length(&element_length);
	assert(S_OK == result);

	enum SymTagEnum elemSymTag;
	result = pElementType->get_symTag((DWORD*)&elemSymTag);
	assert(S_OK == result);

	char valueName[ 128];
	basic_bufferstream<char> formatter(valueName, sizeof(valueName));

	if (SymTagData == elemSymTag || SymTagBaseType == elemSymTag)
	{
		const std::string typeName = dia::GetSymbolTypeName(pElementType);

		for (ULONGLONG i=0; i < length; i += element_length)
		{
			void *ptr = (BYTE*)memInfo.ptr + i;
			SMemoryInfo arrayElem(typeName.c_str(), ptr, (size_t)element_length);

			formatter.seekp(0);
			formatter << "[" << i / element_length << "]" << std::ends;
			MakeProperty_BaseType(pParentProp, valueName, pSymbol, arrayElem);
		}

		pParentProp->Expand(FALSE); // 일단 접어놓는다.
	}
	else // UDT, Array
	{
		const std::string typeName = dia::GetSymbolTypeName(pElementType);
		for (ULONGLONG i=0; i < length; i += element_length)
		{
			formatter.seekp(0);
			formatter << "[" << i / element_length << "]" << std::ends;
			CMFCPropertyGridProperty *pProp =
				new CMFCPropertyGridProperty( common::string2wstring(valueName).c_str() );
			AddProperty( pParentProp, pProp, &STypeData(SymTagUDT,VT_EMPTY,NULL));

			void *ptr = (BYTE*)memInfo.ptr + i;
			SMemoryInfo arrayElem(typeName.c_str(), ptr, (size_t)element_length);
			MakeProperty_Root(pProp, pElementType, arrayElem);
		}

		pParentProp->Expand(FALSE); // 일단 접어놓는다.
	}
}


//------------------------------------------------------------------------
// m_wndPropList 에 Row 를 추가한다.
// pSymbol 은 데이타를 가르키는 심볼이어야 한다.
//------------------------------------------------------------------------
CMFCPropertyGridProperty* visualizer ::MakeProperty_BaseType(
	CMFCPropertyGridProperty *pParentProp, 
	const std::string valueName, IDiaSymbol *pSymbol, 
	const SMemoryInfo &memInfo )
{
	_variant_t value = dia::GetValueFromSymbol(memInfo.ptr, pSymbol);

	// static 변수는 프로세스끼리 공유할 수없으므로 0으로 출력한다.
	bool IsNotPrint = false;
	LocationType locType;
	dia::GetSymbolLocation(pSymbol, &locType);
	if (LocIsStatic == locType)
	{
		value = 0;
		IsNotPrint  = true;
	}

	CMFCPropertyGridProperty *pProp = NULL;
	switch (value.vt)
	{
	case VT_I2:
	case VT_I4:
	case VT_R4:
	case VT_R8:
	case VT_BOOL:
	case VT_DECIMAL:
	case VT_UI2:
	case VT_UI4:
	case VT_INT:
	case VT_UINT:
		{
			pProp = new CMFCPropertyGridProperty( common::string2wstring(valueName).c_str(), value, _T("") );

			_variant_t v1 = pProp->GetValue();
			assert(v1.vt == value.vt);
		}
		break;

	case VT_I1:
	case VT_UI1:
		pProp = new CMFCPropertyGridProperty( common::string2wstring(valueName).c_str(), 
			(_variant_t)(int)0, _T("") );
		break;

	default:
		{
			// 			CMainFrame *pFrm = (CMainFrame*)::AfxGetMainWnd();
			// 			std::stringstream ss;
			// 			ss << valueName << "값의 데이타를 Property애 추가하지 못했습니다.";
			// 			pFrm->GetOutputWnd().AddString( common::string2wstring(ss.str()).c_str() );
		}
		break;
	}
	RETV(!pProp, NULL);

	if (!SetPropertyValue(pProp, value ))
	{
		delete pProp;
		return NULL;
	}

	if (IsNotPrint)
	{
		// 화면에 출력되지 않는 값은 메모리주소를 NULL로 설정해서
		// Refresh 되지 않게 한다.
		pProp->AllowEdit(FALSE);
		pProp->Enable(FALSE);
		AddProperty(pParentProp, pProp, &STypeData(SymTagData, VT_EMPTY, NULL));
	}
	else
	{
		const LONG offset = dia::GetSymbolLocation(pSymbol);
		void *ptr = (BYTE*)memInfo.ptr + offset;
		AddProperty(pParentProp, pProp, &STypeData(SymTagData, value.vt, ptr));
	}

	return pProp;
}


//------------------------------------------------------------------------
// Property 데이타에 value 값을 설정한다. 
//------------------------------------------------------------------------
bool visualizer ::SetPropertyValue(CMFCPropertyGridProperty *pProp, _variant_t value)
{
	switch (value.vt)
	{
	case VT_I2:
	case VT_I4:
	case VT_R4:
	case VT_R8:
	case VT_BOOL:
	case VT_DECIMAL:
	case VT_UI2:
	case VT_UI4:
	case VT_INT:
	case VT_UINT:
	case VT_I1:
	case VT_UI1:
		{
			const size_t optionCnt = pProp->GetOptionCount();
			if (optionCnt > 0)
			{
				//ASSERT_RETV((int)value < optionCnt, false);
				if ((size_t)value < optionCnt)
				{
					_variant_t var = (_variant_t)pProp->GetOption( (int)value );
					pProp->SetValue(var);
				}
			}
			else
			{
				_variant_t var;
				var.ChangeType(pProp->GetValue().vt, &value);
				pProp->SetValue(var);
			}
		}
		break;

		//		{
		//			_variant_t tmp  = (VT_I1 == value.vt)? (int)value.cVal : (int)value.bVal;
		// 			_variant_t var;
		// 			var.ChangeType(pProp->GetValue().vt, &value);
		// 			pProp->SetValue(tmp);
		//  		}
		//  		break;

		// 	case VT_BSTR:
		// 		{
		// 			std::string str;
		// 			operator>>(str);
		// #ifdef _UNICODE
		// 			var.bstrVal = (_bstr_t)common::string2wstring(str).c_str();
		// #else
		// 			var.bstrVal = (_bstr_t)str.c_str();
		// #endif
		// 		}
		// 		break;

	default:
		{
			// Err!!, Property에 value 타입의 값을 넣을 수 없습니다. 
			assert(0);
			return false;
		}
		break;
	}
	return true;
}


//------------------------------------------------------------------------
// Property추가
//------------------------------------------------------------------------
void visualizer ::AddProperty(
								 CMFCPropertyGridProperty *pParentProp, 
								 CMFCPropertyGridProperty *prop, 
								 STypeData *typeData)
{
	RET(!prop);
	RET(!n_pProperty);

	n_pProperty->AddProperty(pParentProp, prop,  typeData);
}
