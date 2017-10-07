/* ===========================================================================
//
// File:            ScoreIO.h
//
// Description:     High Score read/write to a file.
//
// Written by:      Mirza King, April 2001
// ======================================================================== */

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#define File "hs.bin"

char Buffer[8];

/* ---------------------------------------------------------------------------
//
// Name:            ReadHighScore
//
// Function:        Reads binary value from a file and converts it to DWORD.
//
// Return value:    DWORD (High Score)
//
// ------------------------------------------------------------------------ */
DWORD ReadHighScore( void )
{
    DWORD data = 0;

    int hs_file = open( File, O_RDONLY | O_BINARY );

    if( hs_file )
    {
         read( hs_file, Buffer, 4 );
         /***
         Here we want to make sure we operate on BYTE size data !
         ***/
         data = data      | (Buffer[0] & 0xff);
         data = (data<<8) | (Buffer[1] & 0xff);
         data = (data<<8) | (Buffer[2] & 0xff);
         data = (data<<8) | (Buffer[3] & 0xff);

         close( hs_file );
    }

    return data;
}



/* ---------------------------------------------------------------------------
//
// Name:            WriteHighScore
//
// Function:        Converts passed parameter to bytes and writes binary to a
//                  file.
//
// Parameters:      DWORD (High Score)
//
// Return value:    none
//
// ------------------------------------------------------------------------ */

void WriteHighScore( DWORD data )
{
    int hs_file = open( File, O_RDWR | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);

    if( hs_file )
    {
        for( int i=0,k=3; i<4; i++,k-- )
            Buffer[i] = (char) ( data >> 8*k );
              
        write( hs_file, Buffer, sizeof(Buffer) );

        close( hs_file );
    }
}



/* =================================== End ================================ */