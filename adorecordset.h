// ADORecordset.h: interface for the CADORecordset class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADORECORDSET_H__28DC5B61_C9F8_11D5_AE99_00D00961973B__INCLUDED_)
#define AFX_ADORECORDSET_H__28DC5B61_C9F8_11D5_AE99_00D00961973B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CADORecordset  
{
public:
	void SetCursorLocation(int val);
	BOOL IsOpen();
	void  MovePrev();
	void  MoveNext();
	BOOL FindFirst(CString strFind);
	void  MoveFirst();
	void  MoveLast();
	void  AddNew();
	int  GetFieldCount();
	int  GetRecordCount();
	BOOL  IsBOF();
	BOOL  IsEOF();
	void  GetFieldValue(LPCTSTR FldName,_variant_t & retVar);
	void  SetFieldValue(LPCTSTR FldName,_variant_t & var);
	void  GetFieldValue(int index,_variant_t& retVar);
	void  SetFieldValue(int index,_variant_t & var);
	void Close();
	void Edit();
	void Update();
	void Open(CString strCmd,int CursorType=adOpenDynamic,int LockType=adLockOptimistic,int CmdType=adCmdText);
	_ConnectionPtr m_ActiveConnection;
	CADORecordset(_ConnectionPtr pCon);
	CADORecordset();
	virtual ~CADORecordset();
	_RecordsetPtr m_pSet;
};

#endif // !defined(AFX_ADORECORDSET_H__28DC5B61_C9F8_11D5_AE99_00D00961973B__INCLUDED_)
