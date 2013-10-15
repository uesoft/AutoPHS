// DataTableBase.h: interface for the CDataTableBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATATABLEBASE_H__8B24E966_CB5D_4A93_8EB7_FA70AD5BEA93__INCLUDED_)
#define AFX_DATATABLEBASE_H__8B24E966_CB5D_4A93_8EB7_FA70AD5BEA93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DataTableBase  
{
public:
	DataTableBase();
	virtual ~DataTableBase();
public:
	void Initialize(const CString &strSql);
	BOOL IsNull();
	void MovePrev();
	void MoveNext();
	BOOL IsBOF();
	void FindFirst(const CString& strSql);
	CString GetFieldValue(CString strFieldName);
	void Update();
protected:
	_RecordsetPtr m_pRs;

};

#endif // !defined(AFX_DATATABLEBASE_H__8B24E966_CB5D_4A93_8EB7_FA70AD5BEA93__INCLUDED_)
