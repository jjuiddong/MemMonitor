#pragma once


// CDataProperty
namespace visualizer { struct SSymbolInfo; }
class  CDataProperty : public CMFCPropertyGridCtrl
{

public:
	CDataProperty();
	virtual ~CDataProperty();

protected:
	typedef struct _SPropItem
	{
		CMFCPropertyGridProperty *prop;
		std::string typeName;		// dia symbol type name
		STypeData typeData;

	} SPropItem;
	typedef std::list<SPropItem*> PropList;
	typedef PropList::iterator PropItor;

	PropList	m_PropList;

public:
	bool		UpdateProperty(const CString &symbolName);
	void		AddProperty(CMFCPropertyGridProperty *pParentProp, 
		CMFCPropertyGridProperty *prop, const visualizer::SSymbolInfo *pSymbol, 
		STypeData *pTypeData);
	void		Refresh();

protected:
	void		Refresh_Property(CMFCPropertyGridProperty *pProp );
	int		FindOption( const CMFCPropertyGridProperty *pProp, const CString &findStr ) const;
	bool		FindSymbolUpward( CMFCPropertyGridProperty *pProp, OUT visualizer::SSymbolInfo *pOut );

protected:
	DECLARE_MESSAGE_MAP()
public:
	void OnPropertyChanged( CMFCPropertyGridProperty *pProp ) const override;
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


