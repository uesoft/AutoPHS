// Qsort.cpp
//

#include "stdafx.h"
#include "Qsort.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CQsort  Qsort;
void CQsort::QuickSort(long low, long hight)
{
/*Dim tmp As Variant
Dim TMP1 As Long
Dim Partition1
Dim RandIndex
Dim i
Dim j*/
// =============== QuickSort ==================
//   QuickSort works by picking a random "pivot" element in SortArray, then
//   moving every element that is bigger to one side of the pivot, and every
//   element that is smaller to the other side.  QuickSort is then called
//   recursively with the two subdivisions created by the pivot.  Once the
//   number of elements in a subdivision reaches two, the recursive calls end
//   and the array is sorted.
// ======================================
//
	int i,j;
	DATE tmp;
	int tmpix;
	if(low >= hight)
		return;
	i=low;j=hight;
	tmp=sortarray[low];
	tmpix=sortindex[low];
	while(i<j)
	{
		while(i < j && (COleDateTime(sortarray[j]) > COleDateTime(sortarray[i]))) j--;
		if(i < j)
		{
			sortarray[i]=sortarray[j];
			sortindex[i]=sortindex[j];
			i++;
		}
		while(i < j && (COleDateTime(sortarray[i]) <= COleDateTime(tmp))) i++;
		if(i < j)
		{
			sortarray[j]=sortarray[i];
			sortindex[j]=sortindex[i];
			j--;
		}
	}
	sortarray[i]=tmp;
	sortindex[i]=tmpix;
	QuickSort(low,i-1);
	QuickSort(j+1,hight);
}
   /*if( Low < high )
   {

      // Only two elements in this subdivision; swap them if they are out of
      // order, then end recursive calls:
      if (high - Low == 1 )
	  {
		  d1=sortarray[Low];
		  d2=sortarray[high];
         if (d1 > d2)
		 {
            tmp = sortarray[Low];
            sortarray[Low] = sortarray[high];
            sortarray[high] = (DATE)tmp;
            TMP1 = sortindex[Low];
            sortindex[Low] = sortindex[high];
            sortindex[high] = TMP1;
         }
      }

         // Pick a pivot element at random, then move it to the end:
         RandIndex = RandInt(Low, high);
            tmp = sortarray[RandIndex];
            sortarray[RandIndex] = sortarray[high];
            sortarray[high] = tmp;
            TMP1 = sortindex[RandIndex];
            sortindex[RandIndex] = sortindex[high];
            sortindex[high] = TMP1;
         Partition1 = sortarray[high];
         do{


            // Move in from both sides towards the pivot element:
            i = Low; j = high;
			d1=sortarray[i];
			d2=sortarray[j];
            while (i < j && d1 <= Partition1)
               i++;
            while (j > i && d2 >= Partition1)
               j--;

            // If we haven//t reached the pivot element, it means that two
            // elements on either side are out of order, so swap them:
            if( i < j )
			{
               tmp = sortarray[i];
               sortarray[i] = sortarray[j];
               sortarray[j] = tmp;
               TMP1 = sortindex[i];
               sortindex[i] = sortindex[j];
               sortindex[j] = TMP1;
            }
		 }
         while( i < j);

         // Move the pivot element back to its proper place in the array:
               tmp = sortarray[i];
               sortarray[i] = sortarray[high];
               sortarray[high] = tmp;
               TMP1 = sortindex[i];
               sortindex[i] = sortindex[high];
               sortindex[high] = TMP1;
        //
         // Recursively call the QuickSort procedure (pass the smaller
         // subdivision first to use less stack space):
         if ((i - Low) < (high - i)) 
		 {
            QuickSort (Low, i - 1);
            QuickSort (i + 1, high);
         }
		 else
		 {
            QuickSort (i + 1, high);
            QuickSort (Low, i - 1);
		 }
     
   }*/

//DEL int CQsort::RandInt(int lower, int upper)
//DEL {
//DEL 	return  (rand()%100 * (upper - lower + 1))/100 + lower;
//DEL }



CQsort::CQsort()
{
sortarray=NULL;
sortindex=NULL;
}
CQsort::~CQsort()
{
	if(sortarray!=NULL)
	{
		delete [] sortarray;
		sortarray=NULL;
	}
	if(sortindex!=NULL)
	{
		delete [] sortindex;
		sortindex=NULL;
	}
}
