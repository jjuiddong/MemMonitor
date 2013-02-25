// DataProperty.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MemMonitor.h"
#include "DataProperty.h"
#include "Lib/DiaWrapper.h"
#include "visualizer/DefaultPropertyMaker.h"
#include "visualizer/PropertyMaker.h"

using namespace dia;
using namespace visualizer;
using namespace sharedmemory;
using namespace std;


// CDataProperty
CDataProperty::CDataProperty()
{

}

CDataProperty::~CDataProperty()
{
}


BEGIN_MESSAGE_MAP(CDataProperty, CMFCPropertyGridCtrl)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CDataProperty 메시지 처리기입니다.
bool	CDataProperty::UpdateProperty(const CString &symbolName)
{
	RemoveAll();
	BOOST_FOREACH(SPropItem *item, m_PropList)
	{
		SAFE_DELETE(item);
	}
	m_PropList.clear();

	std::string tmpStr = common::wstr2str((LPCWSTR)symbolName);
	std::string str = sharedmemory::ParseObjectName(tmpStr);
	const bool result = visualizer::MakeProperty_DefaultForm(this, tmpStr);

	if (result)
	{
		if (GetPropertyCount() > 0)
		{
			CMFCPropertyGridProperty *prop = GetProperty(0);
			if (prop->GetSubItemsCount() > 0)
				prop->Expand();
		}
	}

	return result;
}

//------------------------------------------------------------------------
// 공유메모리 정보를 갱신한다.
//------------------------------------------------------------------------
void CDataProperty::Refresh()
{
	const int cnt = GetPropertyCount();
	for (int i=0; i < cnt; ++i)
	{
		CMFCPropertyGridProperty *pProp = GetProperty(i);
		Refresh_Property(pProp);
	}
}


//------------------------------------------------------------------------
// 공유메모리 정보를 갱신한다.
//------------------------------------------------------------------------
void	CDataProperty::Refresh_Property(CMFCPropertyGridProperty *pProp )
{
	RET(!pProp);

	if (pProp->IsInPlaceEditing()) // Edit중인 값은 갱신하지 않는다.
		return;

	SPropItem *pItem = (SPropItem*)pProp->GetData();
	if (pItem && 
		(SymTagData == pItem->typeData.symtag 
		|| SymTagBaseType == pItem->typeData.symtag 
		||  SymTagEnum == pItem->typeData.symtag 
		)
		&& pItem->typeData.ptr
		&& pItem->typeData.vt != VT_EMPTY )
	{
		_variant_t value = dia::GetValue(pItem->typeData.ptr, pItem->typeData.vt);
		visualizer::SetPropertyValue(pProp, value);
	}

	const int subCnt = pProp->GetSubItemsCount();
	for (int k=0; k < subCnt; ++k)
	{
		CMFCPropertyGridProperty *p = pProp->GetSubItem(k);
		Refresh_Property(p);
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CDataProperty::OnPropertyChanged( CMFCPropertyGridProperty *pProp ) const
{
	SPropItem *pItem = (SPropItem*)pProp->GetData();
	if (pItem && pItem->typeData.ptr)
	{
		_variant_t curVar = pProp->GetValue();
		_variant_t var;
		if (pProp->GetOptionCount() > 0)
		{
			CString str = (LPCTSTR) (_bstr_t)curVar;
			const int idx = FindOption(pProp, str);
			ASSERT_RET( idx >= 0);
			_variant_t tmp = idx;
			var.ChangeType(VT_INT, &tmp);
		}
		else
		{
 			var.ChangeType(pItem->typeData.vt, &curVar);
		}

		// 메모리에 저장
		dia::SetValue( pItem->typeData.ptr, var);
	}

}


//------------------------------------------------------------------------
// pProp의 Option중에 findVar 와 같은 데이타를 찾는다.
// 없다면 -1을 리턴한다.
// findVar 는 스트링을 가르키는 데이터여야 한다.
//------------------------------------------------------------------------
int CDataProperty::FindOption( const CMFCPropertyGridProperty *pProp, const CString &findStr ) const
{
	RETV(!pProp, -1);

	const int cnt = pProp->GetOptionCount();
	for (int i=0; i < cnt; ++i)
	{
		CString str = (LPCTSTR) (_bstr_t)pProp->GetOption(i);
		if (str == findStr)
			return i;		
	}
	return -1;
}


//------------------------------------------------------------------------
// Property추가
//------------------------------------------------------------------------
void CDataProperty::AddProperty(
								 CMFCPropertyGridProperty *pParentProp, 
								 CMFCPropertyGridProperty *prop, 
								 const SSymbolInfo *pSymbol,
								 STypeData *pTypeData)
{
	RET(!prop);

	SPropItem *p = new SPropItem;
	p->prop = prop;
	p->typeData = *pTypeData;

	if (pSymbol)
	{
		p->typeName = dia::GetSymbolName(pSymbol->pSym);
		p->typeData.ptr = pSymbol->mem.ptr;
	}

	m_PropList.push_back(p);

	prop->SetData((DWORD_PTR)p);

 	if (pParentProp)
	{
		if (!pParentProp->AddSubItem(prop))
		{
			ASSERT(0);
		}
	}
	else
	{
		if (!CMFCPropertyGridCtrl::AddProperty(prop))
		{
//			ASSERT(0);
		}
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CDataProperty::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMFCPropertyGridCtrl::OnLButtonDown(nFlags, point);

	CMFCPropertyGridProperty::ClickArea clickArea;
	CMFCPropertyGridProperty *pProp = HitTest(point,  &clickArea);
	if (CMFCPropertyGridProperty::ClickExpandBox == clickArea)
	{
		int cnt = pProp->GetSubItemsCount();
 		if (pProp->GetSubItemsCount() > 0)
			return;
		SSymbolInfo symbol;
		if (!FindSymbolUpward( pProp, &symbol ))
			return;
		if (visualizer::MakePropertyChild_DefaultForm( this, pProp, symbol))
		{
			pProp->Expand();
			AdjustLayout();
		}
	}
}


//------------------------------------------------------------------------
// find symbol from child to parent node
//------------------------------------------------------------------------
bool	CDataProperty::FindSymbolUpward( CMFCPropertyGridProperty *pProp, OUT SSymbolInfo *pOut )
{
	RETV(!pProp, false);
	RETV(!pOut, false);

	SPropItem *pItemData = (SPropItem*)pProp->GetData();
	RETV(!pItemData, false);

	string name =  pItemData->typeName;
	const int idx = name.find(' ');
	string searchName;
	if (string::npos == idx)
		searchName = name;
	else
		searchName = name.substr(0, idx);

	CMFCPropertyGridProperty *pParentProp = pProp->GetParent();
	if (pParentProp)
	{
		bool retry = false;
		SSymbolInfo symbol;
		if (!FindSymbolUpward( pParentProp, &symbol ))
		{
			retry = true;
		}
		else
		{
			if (!visualizer::Find_ChildSymbol(searchName, symbol, pOut))
				retry = true;
		}

		if (retry)
		{
			// 찾기를 실패했다면, 현재 노드에서 찾기를 시도한다.
			const string typeName = sharedmemory::ParseObjectName(searchName);
			pOut->pSym = CDiaWrapper::Get()->FindType( typeName );
			RETV(!pOut->pSym, false);
			pOut->mem = SMemoryInfo(pItemData->typeName.c_str(), pItemData->typeData.ptr, 0);
		}
	}
	else
	{
		const string typeName = sharedmemory::ParseObjectName(searchName);
		pOut->pSym = CDiaWrapper::Get()->FindType( typeName );
		RETV(!pOut->pSym, false);
		pOut->mem = SMemoryInfo(pItemData->typeName.c_str(), pItemData->typeData.ptr, 0); 
	}
	
	return true;
}


//------------------------------------------------------------------------
// 종료전 초기화
//------------------------------------------------------------------------
void CDataProperty::OnDestroy()
{
	CMFCPropertyGridCtrl::OnDestroy();

	BOOST_FOREACH(SPropItem *item, m_PropList)
	{
		SAFE_DELETE(item);
	}
	m_PropList.clear();
}
