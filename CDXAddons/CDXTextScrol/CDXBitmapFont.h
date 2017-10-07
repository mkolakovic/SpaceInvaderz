//////////////////////////////////////////////////////////////////////////////////
// Project Name:    [ CDX BitmapFont ]
// Original Author: [ Ioannis Karagiorgos - karagior@pluto.fernuni-hagen.de ]
// Author:          [ Jimb Esser - wasteland@writeme.com ]
// Date:            [ 7.4.1999 ]
// Revision:        [ 2.00 ]
//  Updated to work with CDX 3.0 and other bug fixes by Jimb Esser
//  Note: the draw clipped functions have not yet been updated
//////////////////////////////////////////////////////////////////////////////////


#ifndef __CDX_BITMAPFONT__
#define __CDX_BITMAPFONT__

#define CDXINCLUDEALL
#include <cdx.h>


#ifndef CDX_CENTERJUSTIFY
#define CDX_CENTERJUSTIFY 1
#endif
#ifndef CDX_RIGHTJUSTIFY
#define CDX_RIGHTJUSTIFY 3
#endif

// Function used internally that makes a DWORD color value from a RGB triplet
void MakeColor(CDXScreen* Screen, DWORD &c, long r, long g, long b);


class CDXBitmapFont
{
public:

	typedef struct 
	{
		RECT    Rectangle;
		int		Width , Height;
	} CDXBITMAPFONT;

	CDXBITMAPFONT	CDXBitmapFontArray[ 256 ];

	int				FontColor;
	int				FontBackgroundColor;
	int				FontColorKey;
	int				FontHeight;
    int             FontAttributes;
	char			* FontName;
    int             TextSurfaceType;

	CDXSurface *	TextSurface;
   	CDXScreen  *	Screen;

    CDXBitmapFont( CDXScreen * Scr );
	// Note on ColorKey:
	//		if your screen is in 8bit mode, then this parameter must be
	//		the index of the color (dependant on palette), for any other
	//		bitdepth, pass in an RGB(r,g,b) macro parameter
	CDXBitmapFont( CDXScreen * Scr , char * FontNam , int Height , int Color = RGB( 255 , 255 , 255 ), int ColKey = RGB(0,0,0),
				   int BackColor = RGB(0,0,0) , int Attributes = FW_NORMAL , int SurfaceType = CDXMEM_VIDTHENSYS );
	~CDXBitmapFont();

	void Create( CDXScreen * Scr , char * FontNam , int Height , int Color , int ColKey, 
				 int BackColor , int Attributes = FW_NORMAL , int SurfaceType = CDXMEM_VIDTHENSYS );
    void PaintCharactersInSurface( void );
    void PaintCharactersInSurface2( void ); // Prototype of an antialiased version.  Not currently used

	HRESULT Draw(int X, int Y, char * Text , CDXSurface* lpDDest , int Length = -1 );
//    HRESULT DrawClipped(int X, int Y, char * Text , CDXSurface* lpDDest, LPRECT ClipRect , int Length = -1);
    HRESULT DrawTrans(int X, int Y, char * Text , CDXSurface* lpDDest , int Length = -1);
//    HRESULT DrawTransClipped(int X, int Y, char * Text , CDXSurface* lpDDest, LPRECT ClipRect , int Length = -1);

	// iWidth is the width of the box in which the text should be aligned
    HRESULT DrawAligned( int X , int Y , int Width , char * Text , CDXSurface * lpDDest , int Align );
    HRESULT DrawAlignedTrans( int X , int Y , int Width , char * Text , CDXSurface * lpDDest , int Align );

    void SetFont( char * FontNam , int Height , int Color , int Attributes = FW_NORMAL );
    void SetColor( int Color );
    void SetColorKey( int ColorKey );
    void SetBackgroundColor( int Color );

    int GetCharacterWidth( char ch );
    int GetCharacterHeight( char ch );
    int GetTextWidth( char * Text );

    void Restore( );
};

#endif