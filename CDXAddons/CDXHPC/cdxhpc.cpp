/////////////////////////////////////////////////////////////////////
// Project Name:   [ CDX Class Library - CDX.lib ]
// Source File:    [ CDX High Performance Counter Implementation]
// Author:         [ Bil Simser - bsimser@home.com ]
// Contributions:  [ Michael Rich, Jimb Esser ]
// Revision:       [ 2.0 ]
/////////////////////////////////////////////////////////////////////

#include "cdxhpc.h"


/////////////////////////////////////////////////////////////////////
// constructor
/////////////////////////////////////////////////////////////////////
CDXHPC::CDXHPC()
{
	// init to zero
	freq.QuadPart = 0;
	restart.QuadPart = 0;
	bPaused = FALSE;
}

/////////////////////////////////////////////////////////////////////
// starts the counter
/////////////////////////////////////////////////////////////////////
BOOL CDXHPC::Start()
{
	QueryPerformanceCounter(&restart);
	if (!QueryPerformanceFrequency(&freq)) return FALSE;
	// convert it down to milliseconds
	freq.QuadPart /= 1000;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// returns the current # of ms since counter was started
/////////////////////////////////////////////////////////////////////
DWORD CDXHPC::GetValue()
{
	if (bPaused)
		return (DWORD)((paused.QuadPart - restart.QuadPart) / freq.QuadPart);
    LARGE_INTEGER timer;
    QueryPerformanceCounter(&timer);
    return (DWORD)((timer.QuadPart - restart.QuadPart) / freq.QuadPart);
}

/////////////////////////////////////////////////////////////////////
// waits for a specified ms since the counter was started
// note that if its already been that long since it was started
// the function would exit immediately
/////////////////////////////////////////////////////////////////////
void CDXHPC::Wait(DWORD dwMilliSecs)
{
    while (TRUE)
	{
		if (GetValue()>=dwMilliSecs) break;
	}
}

/////////////////////////////////////////////////////////////////////
// waits for a specified ms after the function is called
/////////////////////////////////////////////////////////////////////
void CDXHPC::WaitFor(DWORD dwWaitTime)
{
	DWORD dwStart = GetValue();
	while (TRUE)
	{
		if (GetValue() - dwStart >= dwWaitTime) break;
	}
}


/////////////////////////////////////////////////////////////////////
// resets the counter back to zero
/////////////////////////////////////////////////////////////////////
void CDXHPC::Reset()
{
    LARGE_INTEGER timer, excess;
    QueryPerformanceCounter(&timer);
    excess.QuadPart = (timer.QuadPart - restart.QuadPart) % freq.QuadPart;
    QueryPerformanceCounter(&restart);
    restart.QuadPart -= excess.QuadPart;
}

void CDXHPC::Pause()
{
	if (!bPaused) {
		QueryPerformanceCounter(&paused);
		bPaused = TRUE;
	}
}

void CDXHPC::Resume()
{
	if (bPaused) {
		LARGE_INTEGER timer;
		QueryPerformanceCounter(&timer);
		restart.QuadPart += timer.QuadPart - paused.QuadPart;
		bPaused = FALSE;
	}
}
