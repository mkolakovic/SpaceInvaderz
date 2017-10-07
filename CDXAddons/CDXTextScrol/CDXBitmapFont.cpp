// Project Name:    [ CDX BitmapFont ]
// Original Author: [ Ioannis Karagiorgos - karagior@pluto.fernuni-hagen.de ]
// Author:          [ Jimb Esser - wasteland@writeme.com ]
// Date:            [ 7.4.1999 ]
// Revision:        [ 2.00 ]
//  Updated to work with CDX 3.0 and other bug fixes by Jimb Esser
//////////////////////////////////////////////////////////////////////////////////

#include "CDXBitmapFont.h"

// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
CDXBitmapFont::CDXBitmapFont( CDXScreen * Scr , char * FontNam , int Height , 
                              int Color , int ColKey, int BackColor , int Attributes , int SurfaceType )
{
    TextSurface = NULL;
    Create( Scr , FontNam , Height , Color , ColKey , BackColor , Attributes );
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
CDXBitmapFont::CDXBitmapFont( CDXScreen * Scr )
{
    TextSurface = NULL;
    Create( Scr , "Courier" , 12 , RGB( 255 , 255 , 255 ) , RGB(0,0,0) , RGB(0,0,0) , 
            FW_NORMAL , CDXMEM_VIDTHENSYS);
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
CDXBitmapFont::~CDXBitmapFont( )
{
    if( TextSurface != NULL ) delete TextSurface;    
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
void CDXBitmapFont::Create( CDXScreen * Scr , char * FontNam , int Height , 
                              int Color , int ColKey, int BackColor , int Attributes , int SurfaceType )
{
    int             X , Y , i;
    char            TextBuffer[ 2 ];
    SIZE            size;
    CDXSurface *    TempSurface;

    FontName            = FontNam;
    FontColor           = Color;
    FontBackgroundColor = BackColor;
    FontColorKey        = ColKey;
    FontHeight          = Height;
    FontAttributes      = Attributes;
    TextSurfaceType     = SurfaceType;
    Screen              = Scr;

    if( TextSurface != NULL ) delete TextSurface;

    // create temporary surface in order to get the character widths
    // surface can be created in system memory because it is only needed one time
    TempSurface = new CDXSurface();
	// TODO: Errorcheck
	TempSurface->Create( Screen , 100 , 100 , CDXMEM_SYSTEMONLY );

    // the font surface will be width=320 and height as needed
    // i chose width=320 because some video cards dont support surfaces wider
    // than the actual resolution in video memory, so if you choose 
    // videomode 320x200 or 320x240 the surface would be allocated in system memory
    // which would slow down output of fonts.
    TempSurface->ChangeFont( FontName , 0 , FontHeight , FontAttributes );

    // loop through all 256 characters and calculate position and size of every character
 
    X = Y = 0;

    for( i=0; i<128; i++ )
    {
        TextBuffer[ 0 ] = i;
        TextBuffer[ 1 ] = '\0';

        HDC dc = TempSurface->GetDC( );
        TempSurface->SetFont( );
        SetBkMode( dc , OPAQUE );
        SetTextColor( dc, RGB( 255 , 255 , 255 ) );
        SetBkColor( dc, RGB( 0 , 0 , 0 ) );
        GetTextExtentPoint32( dc , TextBuffer , 1 , &size );
        TempSurface->ReleaseDC( );

        // if x + character width exceed 310 then begin new line
        if( X+size.cx >= 310 )
        {
            X = 0;
            Y += size.cy;
        }

        CDXBitmapFontArray[ i ].Height = size.cy;
        CDXBitmapFontArray[ i ].Width  = size.cx;

        CDXBitmapFontArray[ i ].Rectangle.left      = X;
        CDXBitmapFontArray[ i ].Rectangle.top       = Y;
        CDXBitmapFontArray[ i ].Rectangle.right     = X + size.cx;
        CDXBitmapFontArray[ i ].Rectangle.bottom    = Y + size.cy;
        X += size.cx;
    }


    delete TempSurface;

    if( TextSurface != NULL )
        delete TextSurface;

    TextSurface = new CDXSurface();
	// TODO: Errorcheck
	TextSurface->Create( Screen , 320 , Y+Height+5 , TextSurfaceType );
    PaintCharactersInSurface( );
}

void GetRGB(CDXScreen* Screen, DWORD c, long &r, long &g, long &b)
{
	int r2, g2, b2;
	switch (Screen->GetBPP()) {
	case 8:
		Screen->GetColor(c, &r2, &g2, &b2);
		r = r2;
		g = g2;
		b = b2;
		break;
	case 15:
		b = c & 0x1f;
		g = (c >> 5) & 0x1f;
		r = (c >> 10) & 0x1f;
		break;
	case 16:
		b = c & 0x1f;
		g = (c >> 5) & 0x3f;
		r = (c >> 11) & 0x1f;
		break;
	case 24:
		b = c & 0xFF;
		g = (c >> 8) & 0xFF;
		r = (c >> 16) & 0xFF;
		break;
	case 32:
		b = c & 0xFF;
		g = (c >> 8) & 0xFF;
		r = (c >> 16) & 0xFF;
		break;
	}
}
void MakeColor(CDXScreen* Screen, DWORD &c, long r, long g, long b)
{
	switch (Screen->GetBPP()) {
	case 8:
		c = (r) | (g << 8) | (b << 16); // Not Supported, return it as it was initially encoded...
		break;
	case 15:
		c = (b) | (g << 5) | (r << 10);
		break;
	case 16:
		c = (b) | (g << 5) | (r << 11);
		break;
	case 24:
	case 32:
		c = (b) | (g << 8) | (r << 16);
		break;
	}
}



// --------------------------------------------------------------------------------------
//  rough draft of a anti-aliasing routine.  Not currently used anyways
//  @author Jimb Esser
// --------------------------------------------------------------------------------------
void CDXBitmapFont::PaintCharactersInSurface2( void )
{
    char    TextBuffer[ 2 ];
    int     i;

    TextSurface->ChangeFont( FontName , 0 , FontHeight, FontAttributes );
    TextSurface->SetColorKey( FontColorKey );

    // create temporary surface in order to draw the fonts before
	// antialiasing
    CDXSurface *TempSurface = new CDXSurface();
	// TODO: Errorcheck
	TempSurface->Create( Screen , 200 , 200 , CDXMEM_SYSTEMONLY );
    TempSurface->ChangeFont( FontName , 0 , FontHeight*2 , FontAttributes );
	TempSurface->Fill(FontBackgroundColor);

    // print all characters on surface

    HRGN Region;
	HDC dc;

//	dc = TextSurface->GetDC( );
//	TextSurface->SetFont( );
//	TextSurface->ReleaseDC();

	int iHeight, iWidth, x, y;
	DWORD c;
	long r, g, b;
	long dr, dg, db;

    for( i=0; i<128; i++ )
    {
        TextBuffer[ 0 ] = i;
        TextBuffer[ 1 ] = '\0';

		// Draw the text
		TempSurface->Fill(0);
		dc = TempSurface->GetDC( );
		TempSurface->SetFont( );
		SetBkMode( dc , OPAQUE );
		SetTextColor( dc,  FontColor );
		SetBkColor( dc, FontBackgroundColor );

        // select clip region because some characters like 'j' exceed the drawing rectangle
        // and reach into other characters bitmaps
        Region = CreateRectRgn( CDXBitmapFontArray[i].Rectangle.left ,
                                CDXBitmapFontArray[i].Rectangle.top ,
                                CDXBitmapFontArray[i].Rectangle.right ,
                                CDXBitmapFontArray[i].Rectangle.bottom );
        // SelectClipRgn( dc, Region );
        TextOut( dc, 0, 0, TextBuffer , 1 );

	    TempSurface->ReleaseDC( );

		// Sample down to half size...
		iWidth = CDXBitmapFontArray[i].Rectangle.right - CDXBitmapFontArray[i].Rectangle.left;
		iHeight = CDXBitmapFontArray[i].Rectangle.bottom - CDXBitmapFontArray[i].Rectangle.top;
		for (x=0; x<iWidth; x++)
			for (y=0; y<iHeight; y++) {
				TempSurface->Lock();
				c = TempSurface->GetPixel(x*2, y*2);
				GetRGB(Screen, c, dr, dg, db);
				r = dr; g = dg; b = db;

				c = TempSurface->GetPixel(x*2+1, y*2);
				GetRGB(Screen, c, dr, dg, db);
				r += dr; b += db; g += dg;

				c = TempSurface->GetPixel(x*2, y*2+1);
				GetRGB(Screen, c, dr, dg, db);
				r += dr; b += db; g += dg;

				c = TempSurface->GetPixel(x*2+1, y*2+1);
				GetRGB(Screen, c, dr, dg, db);
				r += dr; b += db; g += dg; /* */

				TempSurface->UnLock();

				r /=4; g /=4; b /=4;
				MakeColor(Screen, c, r, g, b);

				TextSurface->Lock();
				TextSurface->PutPixel(x+CDXBitmapFontArray[i].Rectangle.left, y+CDXBitmapFontArray[i].Rectangle.top, c);
				TextSurface->UnLock();
			}


        DeleteObject( Region );
    }


//	TextSurface->UnLock();
    TextSurface->SetColorKey( FontColorKey );
}


void CDXBitmapFont::PaintCharactersInSurface( void )
{
    char    TextBuffer[ 2 ];
    int     i;

    TextSurface->ChangeFont( FontName , 0 , FontHeight , FontAttributes );

	// Convert the font color to what it should be for the
	// bitdepth of the screen.  If we're in 8-bit, nothing
	// will be done with it, assume a palette color index
	// is passed in.  If MakeColor() in cdxscreen (or here)
	// is implemented to handle 8-bit, we could process
	// 8-bit like the others (taking an RGB() parameter).
	int r, g, b;
		r = FontColorKey & 0xFF;
		g = (FontColorKey >> 8) & 0xFF;
		b = (FontColorKey >> 16) & 0xFF;
	DWORD c;
	MakeColor(Screen, c, r, g, b);
    TextSurface->SetColorKey( c );

    // print all characters on surface
    HDC dc = TextSurface->GetDC( );
    TextSurface->SetFont( );
    SetBkMode( dc , OPAQUE );
    SetTextColor( dc,  FontColor );
    SetBkColor( dc, FontBackgroundColor );

    HRGN Region;

    for( i=0; i<128; i++ )
    {
        TextBuffer[ 0 ] = i;
        TextBuffer[ 1 ] = '\0';

        // select clip region because some characters like 'j' exceed the drawing rectangle
        // and reach into other characters bitmaps
        Region = CreateRectRgn( CDXBitmapFontArray[i].Rectangle.left ,
                                CDXBitmapFontArray[i].Rectangle.top ,
                                CDXBitmapFontArray[i].Rectangle.right ,
                                CDXBitmapFontArray[i].Rectangle.bottom );
        SelectClipRgn( dc, Region );
        TextOut( dc, CDXBitmapFontArray[i].Rectangle.left , 
                                     CDXBitmapFontArray[i].Rectangle.top , TextBuffer , 1 );
        DeleteObject( Region );
    }


    TextSurface->ReleaseDC( );
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
HRESULT CDXBitmapFont::DrawAligned( int X , int Y , int Width , char * Text , CDXSurface * lpDDest , int Align )
{
    int     TWidth, Width2;
    HRESULT rval;
    char    ch;

    TWidth = GetTextWidth( Text );

    if( Align == CDX_CENTERJUSTIFY )
    {
        // if text wider then output width do print only the fitting characters
        if( TWidth < Width )
        {
            X = X + ( Width - TWidth ) / 2;

            while( (ch = *Text++) != '\0' )
            {
                rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
                                                 DDBLTFAST_WAIT);
    
                if(rval == DDERR_SURFACELOST) 
                {
                    Restore( );
					rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
													 DDBLTFAST_WAIT);
	                if(rval == DDERR_SURFACELOST) 
						return rval;
                }
				X += CDXBitmapFontArray[ ch ].Width;
            }
        }
        else
        {
            TWidth = 0;
            while( (ch = *Text++) != '\0' )
            {
                Width2 = CDXBitmapFontArray[ ch ].Width;
                TWidth += Width2;
                if( TWidth > Width )
                    break;
                rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
                                                 DDBLTFAST_WAIT);   
                if(rval == DDERR_SURFACELOST) 
                {
                    Restore( );
					rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
													 DDBLTFAST_WAIT);   
					if (rval ==	DDERR_SURFACELOST)
						return rval;
                }
				X += Width2;
            }
        }
    }
    else
        if( Align == CDX_RIGHTJUSTIFY )
        {
            // if text wider then output width do print only the fitting characters
            if( TWidth < Width )
            {
                X = X + ( Width - TWidth );

                while( (ch = *Text++) != '\0' )
                {
                    rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
                                             DDBLTFAST_WAIT);
                    if(rval == DDERR_SURFACELOST) 
                    {
                        Restore( );
	                    rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
                                             DDBLTFAST_WAIT);
						if (rval ==	DDERR_SURFACELOST)
							return rval;
                    }
					X += CDXBitmapFontArray[ ch ].Width;
                }
            }
            else
            {
                TWidth = 0;
                while( (ch = *Text++) != '\0' )
                {
                    Width2 = CDXBitmapFontArray[ ch ].Width;
                    TWidth += Width2;
                    if( TWidth > Width )
                        break;
                    rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
                                                     DDBLTFAST_WAIT);   
                    if(rval == DDERR_SURFACELOST) 
                    {
                        Restore( );
	                    rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
                                                     DDBLTFAST_WAIT);   
						if (rval ==	DDERR_SURFACELOST)
							return rval;
                    }
					X += Width2;
                }
            }
        }

    return rval;
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
HRESULT CDXBitmapFont::DrawAlignedTrans( int X , int Y , int Width , char * Text , CDXSurface * lpDDest , int Align )
{
    int     TWidth, Width2;
    HRESULT rval;
    char    ch;

    TWidth = GetTextWidth( Text );

    if( Align == CDX_CENTERJUSTIFY )
    {
        // if text wider then output width do print only the fitting characters
        if( TWidth < Width )
        {
            X = X + ( Width - TWidth ) / 2;

            while( (ch = *Text++) != '\0' )
            {
                rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
                                                 DDBLTFAST_WAIT| DDBLTFAST_SRCCOLORKEY);
    
                if(rval == DDERR_SURFACELOST) 
                {
                    Restore( );
	                rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
                                                 DDBLTFAST_WAIT| DDBLTFAST_SRCCOLORKEY);
					if (rval ==	DDERR_SURFACELOST)
						return rval;
                }
				X += CDXBitmapFontArray[ ch ].Width;
            }
        }
        else
        {
            TWidth = 0;
            while( (ch = *Text++) != '\0' )
            {
                Width2 = CDXBitmapFontArray[ ch ].Width;
                TWidth += Width2;
                if( TWidth > Width )
                    break;
                rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
                                                 DDBLTFAST_WAIT| DDBLTFAST_SRCCOLORKEY);   
                if(rval == DDERR_SURFACELOST) 
                {
                    Restore( );
	                rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
		                                             DDBLTFAST_WAIT| DDBLTFAST_SRCCOLORKEY);   
					if (rval ==	DDERR_SURFACELOST)
						return rval;
                }
				X += Width2;
            }
        }
    }
    else
        if( Align == CDX_RIGHTJUSTIFY )
        {
            // if text wider then output width do print only the fitting characters
            if( TWidth < Width )
            {
                X = X + ( Width - TWidth );

                while( (ch = *Text++) != '\0' )
                {
                    rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
                                             DDBLTFAST_WAIT| DDBLTFAST_SRCCOLORKEY);
                    if(rval == DDERR_SURFACELOST) 
                    {
                        Restore( );
	                    rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
		                                         DDBLTFAST_WAIT| DDBLTFAST_SRCCOLORKEY);
						if (rval ==	DDERR_SURFACELOST)
							return rval;
                    }
					X += CDXBitmapFontArray[ ch ].Width;
                }
            }
            else
            {
                TWidth = 0;
                while( (ch = *Text++) != '\0' )
                {
                    Width2 = CDXBitmapFontArray[ ch ].Width;
                    TWidth += Width2;
                    if( TWidth > Width )
                        break;
                    rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
                                                     DDBLTFAST_WAIT| DDBLTFAST_SRCCOLORKEY);   
                    if(rval == DDERR_SURFACELOST) 
                    {
                        Restore( );
	                    rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
		                                                 DDBLTFAST_WAIT| DDBLTFAST_SRCCOLORKEY);   
						if (rval ==	DDERR_SURFACELOST)
							return rval;
                    }
					X += Width2;
                }
            }
        }

    return rval;
}



// --------------------------------------------------------------------------------------
// Length = -1 means draw the complete string, else length characters
// --------------------------------------------------------------------------------------
HRESULT CDXBitmapFont::Draw(int X, int Y, char * Text , CDXSurface* lpDDest , int Length )
{
	HRESULT rval;
	char    ch;

	while(  ( (ch = *Text++) != '\0' ) &&
			(  Length--      != 0    )    )
	{
		rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
				DDBLTFAST_WAIT);

		if(rval == DDERR_SURFACELOST) 
		{
			Restore( );
			rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
					DDBLTFAST_WAIT);
			if (rval == DDERR_SURFACELOST)
				return rval;
		}
		X += CDXBitmapFontArray[ ch ].Width;
	}

	return rval;
}



// --------------------------------------------------------------------------------------
// Length = -1 means draw the complete string, else length characters
// --------------------------------------------------------------------------------------
/*HRESULT CDXBitmapFont::DrawClipped(int X, int Y, char * Text , CDXSurface* lpDDest, LPRECT ClipRect , int Length )
{
    HRESULT rval;
    RECT    ModSrc;
    int     XSave , YSave;
    char    ch;

    while(  ( (ch = *Text++) != '\0' ) &&
            (  Length--      != 0    )    )

    {
        ModSrc = CDXBitmapFontArray[ ch ].Rectangle;
        XSave = X; YSave = Y;
        Clip(&X, &Y, &ModSrc, ClipRect);
        rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &ModSrc, 
                                         DDBLTFAST_WAIT);

        if(rval == DDERR_SURFACELOST) 
        {
            Restore( );        
	        rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &ModSrc, 
		                                     DDBLTFAST_WAIT);
			if (rval ==	DDERR_SURFACELOST)
				return rval;
        }

		X = XSave;
		Y = YSave;

		X += CDXBitmapFontArray[ ch ].Width;
    }

    return rval;

}
*/



// --------------------------------------------------------------------------------------
// Length = -1 means draw the complete string, else length characters
// --------------------------------------------------------------------------------------
HRESULT CDXBitmapFont::DrawTrans(int X, int Y, char * Text , CDXSurface* lpDDest , int Length )
{
    HRESULT rval;
    char    ch;

    while(  ( (ch = *Text++) != '\0' ) &&
            (  Length--      != 0    )    )

    {
        rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
                                         DDBLTFAST_WAIT| DDBLTFAST_SRCCOLORKEY);

        if(rval == DDERR_SURFACELOST) 
        {
            Restore( );
	        rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &CDXBitmapFontArray[ ch ].Rectangle, 
                                         DDBLTFAST_WAIT| DDBLTFAST_SRCCOLORKEY);
        }
		X += CDXBitmapFontArray[ ch ].Width;
    }

    return rval;
}



// --------------------------------------------------------------------------------------
// Length = -1 means draw the complete string, else length characters
// --------------------------------------------------------------------------------------
/*HRESULT CDXBitmapFont::DrawTransClipped(int X, int Y, char * Text , CDXSurface* lpDDest, LPRECT ClipRect , int Length )
{
    HRESULT rval;
    RECT    ModSrc;
    int     XSave , YSave;
    char    ch;

    while(  ( (ch = *Text++) != '\0' ) &&
            (  Length--      != 0    )    )

    {
        ModSrc = CDXBitmapFontArray[ ch ].Rectangle;
        XSave = X; YSave = Y;
        Clip(&X, &Y, &ModSrc, ClipRect);
        rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &ModSrc, 
                                         DDBLTFAST_WAIT| DDBLTFAST_SRCCOLORKEY);

        if(rval == DDERR_SURFACELOST) 
        {
            Restore( );
			rval = lpDDest->GetDDS()->BltFast(X, Y, TextSurface->GetDDS(), &ModSrc, 
											 DDBLTFAST_WAIT| DDBLTFAST_SRCCOLORKEY);
			if (rval ==	DDERR_SURFACELOST)
				return rval;
        }
		X = XSave;
		Y = YSave;
		X += CDXBitmapFontArray[ ch ].Width;
    }

    return rval;

}
*/


// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
void CDXBitmapFont::SetFont( char * FontNam , int Height , int Color , int Attributes )
{
    Create( Screen , FontNam , Height , Color , FontColorKey , FontBackgroundColor , Attributes );
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
void CDXBitmapFont::SetColor( int Color )
{
    FontColor      = Color;

    PaintCharactersInSurface( );
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
void CDXBitmapFont::SetColorKey( int ColorKey )
{
    FontColorKey = ColorKey;

    TextSurface->SetColorKey( FontColorKey );
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
void CDXBitmapFont::SetBackgroundColor( int Color )
{
    FontBackgroundColor = Color;

    PaintCharactersInSurface( );
}




// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
int CDXBitmapFont::GetCharacterWidth( char ch )
{
    return CDXBitmapFontArray[ ch ].Width;
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
int CDXBitmapFont::GetCharacterHeight( char ch )
{
    return CDXBitmapFontArray[ ch ].Height;
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
int CDXBitmapFont::GetTextWidth( char * Text )
{
    int  Result;
    char ch;

    Result = 0;

    while( (ch = *Text++) != '\0' )
    {
        Result += CDXBitmapFontArray[ ch ].Width;
    }

    return Result;
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
void CDXBitmapFont::Restore( void )
{
    TextSurface->Restore();
    PaintCharactersInSurface( );
}
