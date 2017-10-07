/* ===========================================================================
//
// File:            Debug.h
//
// Description:     Prints debug info..
//
// Written by:      Mirza King, April 2001
// ======================================================================== */

#define CDXINCLUDEALL
#include <CDX.H>


/* ---------------------------------------------------------------------------
//
// Name:        dprints
//
// Function:    Prints debug info on the screen starting in the upper left 
//              corner.
//
// Remarks:     Writes in red color.  Screen object must exist.
//
// Usage:       dprints("mirza king %d", 2001); 
//
// ------------------------------------------------------------------------ */
void dprints( char *fmt, ... )
{
	char buf[256];

    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

	    Screen->GetBack()->GetDC();
    	Screen->GetBack()->TextXY(0, 0,RGB(255,0,0) , buf);
	    Screen->GetBack()->ReleaseDC();

}




/* ---------------------------------------------------------------------------
//
// Name:        dprints
//
// Function:    Prints debug info to a file.
//
// Remarks:     File handle must exist.
//
// Usage:       dprintf("mirza king %d \n", 2001); 
//
// ------------------------------------------------------------------------ */
void dprintf(FILE *fdebug, char *fmt, ... )
{

    char buf[256];

    if (DebugFlag == 0) return;
    
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    if (fdebug != NULL)
    {
        fprintf(fdebug, buf);
    }
    
}


/* =============== End of file Debug.h ==================================== */
