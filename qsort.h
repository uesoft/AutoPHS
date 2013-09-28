// Qsort.h
//

#if !defined(AFX_QSORT_H__66EB6429_712C_11D5_8752_00D009619765__INCLUDED_)
#define AFX_QSORT_H__66EB6429_712C_11D5_8752_00D009619765__INCLUDED_


#include "resource.h"

#include <time.h>
class CQsort
{
public:
	CQsort();
	~CQsort();
     void QuickSort(LONG Low, LONG high);
    /*
     *  ================ RandInt ==================
     *    Returns a random integer greater than or equal to the Lower
     *    Rparameter
     *    and less than or equal to the Upper parameter.
     *  ======================================
     */
	 DATE* sortarray;
	 int* sortindex;
protected:


private:


};
extern CQsort  Qsort;
#endif /* Qsort_h */
