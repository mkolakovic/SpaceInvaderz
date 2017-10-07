// CDXHPC class, with additional functions - Pause and Resume added by Jimb Esser

#ifndef _H_CDXHPC
#define _H_CDXHPC

#define WIN32_EXTRA_LEAN
#include <windows.h>

class CDXHPC
{
public:
	// constructor
	CDXHPC();
	// destructor
	virtual ~CDXHPC() {};

	// starts the counter
	BOOL Start();
	// returns the current # of ms since counter was started
	DWORD GetValue();
	// waits for a specified ms since the counter was started
	// note that if its already been that long since it was started
	// the function would exit immediately
	void Wait(DWORD dwStopTime);
	// waits for a specified ms after the function is called
	void WaitFor(DWORD dwWaitTime);

	// resets the counter back to zero
	void Reset();

	void Pause();
	void Resume();

protected:
	// counter variables
	LARGE_INTEGER freq, restart, paused;
	BOOL bPaused;
};

#endif		/* #ifndef _H_CDXHPC */
