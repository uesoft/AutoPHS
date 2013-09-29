// ImportPlantVision.h: interface for the CImportPlantVision class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMPORTPLANTVISION_H__ADFC3CEC_6E2E_440E_88C6_21C1F5656591__INCLUDED_)
#define AFX_IMPORTPLANTVISION_H__ADFC3CEC_6E2E_440E_88C6_21C1F5656591__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "importfromxlsdlg.h"
 
class CImportPlantVision : public CImportFromXLSDlg  
{

public:
	virtual BOOL InitPropertyWnd();
	void BeginImport();
	CImportPlantVision();
	virtual ~CImportPlantVision();

protected:
	virtual void SaveSetToReg();
};

#endif // !defined(AFX_IMPORTPLANTVISION_H__ADFC3CEC_6E2E_440E_88C6_21C1F5656591__INCLUDED_)
