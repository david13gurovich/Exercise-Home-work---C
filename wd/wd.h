#ifndef __H_WD__
#define __H_WD__

typedef enum 
{
	WD_SUCCESS = 0,
	WD_ALLOC_FAILED = 1,
	WD_FAILED = 2
} status_wd;

status_wd StartWd(int argc, char const *argv[], void **params);
/********************************************************************
* WdStart - the WD detect and recover from program malfunctions.
*
* argv - program's arguments monitored by watch dog. 
*
* param - is used to hold information, taken by StopWd.
*
* Return - 0 if WD_SUCCESS, 1 if WD_ALLOC_FAILED, 2 if WD_FAILED,
********************************************************************/

void StopWd(void *params);
/********************************************************************
* WdStart - Stop the Watch Dog action.
*
* param - is used to hold information, given by StartWd.
********************************************************************/

#endif