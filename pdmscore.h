#ifndef _PDMSCORE_H
#define _PDMSCORE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



BOOL	PDMSToPHS(_RecordsetPtr rsZA,CString strFileName);
void ReadPDMS();
BOOL PHSToPDMS(CString strFileName);

#endif