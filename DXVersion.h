/* ===========================================================================
//
// File:            DXVersion.h
//
// Description:     As soon as the application starts, we will read byte from 
//                  "dx.bin" file. If that byte is equal to "1" (means the last 
//                  time we ran this application it was shut down properly) write
//                  0 byte to "dx.bin" file.  Upon exiting application normaly we
//                  will write "1". In case we read "0" AND GetDXVersion < 7,  at
//                  the start-up we will give user a warning to download the 
//                  latest version of DirectX since that might be a reason 
//                  application crashed.  
//  Note: The first time application is running on a given system, "dx.bin" should 
//        exist and have value "1" written in it !  
//
// Written by:      Mirza King, April 2001
// ======================================================================== */

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>



typedef HRESULT(WINAPI * DIRECTDRAWCREATEEX)( GUID*, VOID**, REFIID, IUnknown* );


/* ---------------------------------------------------------------------------
//
// Name:          GetDXVersion()      
//
// Function:        Determine if DirectX 7.0 or higher is installed.
//
// Return value:    DirectX Version 0 or 7 ( BYTE )
//
// ------------------------------------------------------------------------ */
BYTE GetDXVersion()
{

    DIRECTDRAWCREATEEX   DirectDrawCreateEx = NULL;
    HINSTANCE            hDDrawDLL          = NULL;
    BYTE                 dwDXVersion        = 0;

    // First see if DDRAW.DLL even exists.
    hDDrawDLL = LoadLibrary( "DDRAW.DLL" );
    if( hDDrawDLL == NULL )
    {
        dwDXVersion = 0;
        return dwDXVersion;
    }

	//-------------------------------------------------------------------------
    // DirectX 7.0 Check
	//-------------------------------------------------------------------------

    // Check for DirectX 7 by creating a DDraw7 object
    LPDIRECTDRAW7 pDD7;
    DirectDrawCreateEx = (DIRECTDRAWCREATEEX)GetProcAddress( hDDrawDLL,
                                                       "DirectDrawCreateEx" );
    if( NULL == DirectDrawCreateEx )
    {
        FreeLibrary( hDDrawDLL );
        return dwDXVersion;
    }

    if( FAILED( DirectDrawCreateEx( NULL, (VOID**)&pDD7, IID_IDirectDraw7,
                                    NULL ) ) )
    {
        FreeLibrary( hDDrawDLL );
        return dwDXVersion;
    }

    // DDraw7 was created successfully. We must be at least DX7.0
    dwDXVersion = 0x7;
    pDD7->Release();


	//-------------------------------------------------------------------------
    // End of checking for versions of DirectX 
	//-------------------------------------------------------------------------

    // Close open libraries and return
    FreeLibrary( hDDrawDLL );
    
    return dwDXVersion;
}



/* ---------------------------------------------------------------------------
//
// Name:           ReadExitStatus( void ) 
//
// Function:       Read BYTE binary from a file.   
//
// Return value:    BYTE
//
// ------------------------------------------------------------------------ */
BYTE ReadExitStatus( void )
{
    char Buffer[2];
    BYTE data = 0;

    int status_file = open( "dx.bin", O_RDONLY | O_BINARY );

    if( status_file )
    {
         read( status_file, Buffer, 1 );
         /***
         Here we want to make sure we operate on BYTE size data !
         ***/
         data = (Buffer[0] & 0xff);
         close( status_file );
    }

    return data;
}



/* ---------------------------------------------------------------------------
//
// Name:          WriteExitStatus( BYTE data )   
//
// Function:      Write BYTE binary to a file.  
//                  
//
// Parameters:      BYTE ( data )
//
// Return value:    none
//
// ------------------------------------------------------------------------ */

void WriteExitStatus( BYTE data ) 
{
    char Buffer[2];

    int status_file = open( "dx.bin", O_RDWR | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);

    if( status_file )
    {
        Buffer[0] = data;
        Buffer[1] = '\0';
        write( status_file, Buffer, sizeof(Buffer) );
        close( status_file );
    }
}


/* ================================ End ==================================== */