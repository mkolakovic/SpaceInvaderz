/* ===========================================================================
//
// OptionsFile:            Options.h
//
// Description:     Game options - read/write to a file.
//
// Written by:      Mirza King, April 2001
// ======================================================================== */

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#define OptionsFile "ops.bin"

char OptionsBuffer[8];

/* ---------------------------------------------------------------------------
//
// Name:            ReadOptions
//
// Function:        Reads binary value from a file and converts it to DWORD.
//
// Return value:    DWORD (High Score)
//dprints("data =  %X   first = %X   second = %X", data, (data&0xff00)>>8, data&0xff);
// ------------------------------------------------------------------------ */
DWORD ReadOptions( void )
{
    DWORD data = 0;

    int file = open( OptionsFile, O_RDONLY | O_BINARY );

    if( file )
    {
         read( file, OptionsBuffer, 4 );
         /***
         Here we want to make sure we operate on BYTE size data !
         ***/
         data = data      | (OptionsBuffer[0] & 0xff);
         data = (data<<8) | (OptionsBuffer[1] & 0xff);
         data = (data<<8) | (OptionsBuffer[2] & 0xff);
         data = (data<<8) | (OptionsBuffer[3] & 0xff);

         close( file );
    }

    return data;
}



/* ---------------------------------------------------------------------------
//
// Name:            WriteOptions
//
// Function:        Converts passed parameter to bytes and writes binary to a
//                  file.
//
// Parameters:      DWORD 
//
// Return value:    none
//bool fisrst,second;
//WriteOptions( (((unsigned)first&0xff)<<8) | ((unsigned)second&0xff) );
//
//
// ------------------------------------------------------------------------ */

void WriteOptions( DWORD data )
{
    int file = open( OptionsFile, O_RDWR | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);

    if( file )
    {
        for( int i=0,k=3; i<4; i++,k-- )
            OptionsBuffer[i] = (char) ( data >> 8*k );
              
        write( file, OptionsBuffer, sizeof(OptionsBuffer) );

        close( file );
    }
}



/* =================================== End ================================ */