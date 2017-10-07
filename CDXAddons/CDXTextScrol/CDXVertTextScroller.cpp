// Project Name:   [ CDXVertTextScroller ]
// Author:         [ Ioannis Karagiorgos - karagior@pluto.fernuni-hagen.de ]
// Date:           [ 7.4.1999 ]
// Revision:       [ 1.00 ]
//////////////////////////////////////////////////////////////////////////////////

#include "CDXVertTextScroller.h"



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
CDXVertTextScroller::CDXVertTextScroller( CDXScreen * Scr , RECT * ClipRectangle , 
											char * ScrollText , int TextJustif , 
                                            int LineSpace , CDXBitmapFont * Font )
{  
    Text          = NULL;
	TextSurface   = NULL;
	Screen	      = Scr;
    TextJustify   = TextJustif;
    LineSpacing   = LineSpace;
    LineHeight    = 0;
    TextHeight    = 0;
    IY            = 0;


    Text = new char [strlen( ScrollText )+1 ];
    if( strlen( ScrollText ) == 0 )
        strcpy( Text , " " );
    else
	    strcpy( Text , ScrollText );

    if( Font == NULL )
    {
        TextSurface = new CDXBitmapFont( Screen );
        OwnFont     = true; // class has to destroy font by herself
    }
    else
    {
        TextSurface = Font;
        OwnFont     = false; // font has to be destroyed by creator, not by this class
    }

    ClipRect.left   = ClipRectangle->left;
    ClipRect.top    = ClipRectangle->top;
    ClipRect.right  = ClipRectangle->right;
    ClipRect.bottom = ClipRectangle->bottom;

    StartX = ClipRect.left;
    StartY = ClipRect.top;

    TWidth        = ClipRect.right  - StartX;
    THeight       = ClipRect.bottom - StartY;

	BottomEnd	  = FALSE;
	TopEnd		  = FALSE;

    Create( );

    ResetPosition( );
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void CDXVertTextScroller::Create( void )
{
    int     ChWidth , Width , Width2 , LineStart , Index , Index2, i , LineWidth;
    bool    found;
    char    ch;
    char    TextArray[ 1024 ];

    LineCount = 1;
    Index = 0;
    Width = 0;
    LineStart = 0;

    // now begin initializing lines array
    for( i=0; i<CDXVERTSCROLLERMAXLINES; i++ )
    {
        Lines[i] = new CDXVertScrollerStruct;
        Lines[i]->NumberOfChars = 0;
        Lines[i]->LineWidth     = 0;
        Lines[i]->Characters    = NULL;
        Lines[i]->ExtraSpace    = NULL;
    }

    LineCount = 0;
    Index = 0;
    Width = 0;
    LineStart = 0;

    for( ; ; )
    {
        ch = Text[ Index ];

        // test for linefeed
        if( ( ch == '\n' )  || ( ch == '\0' ) )
        {
            LineWidth = Index - LineStart;
            for( i=0; i<LineWidth; i++ )
                TextArray[ i ] = Text[ LineStart+i] ;
            TextArray[ i ] = '\0';
            
            Lines[ LineCount ]->LineWidth = Width;
            Lines[ LineCount ]->NumberOfChars = LineWidth;
            Lines[ LineCount ]->Characters = new char[ LineWidth + 1 ];
            strcpy( Lines[ LineCount ]->Characters , TextArray );
            ++ Index;
            LineStart = Index;
            Width = 0;
            ++ LineCount;

            if( ch == '\n' )
                continue;
            else
                break;
        }

        ChWidth = TextSurface->GetCharacterWidth( ch );

        // check if character fits in current line
        if( Width + ChWidth > TWidth )
        { // no, go to next line
            found = false;
            // trace back to see, if we can break the whole word into the next line
            Index2 = Index;
            Width2 = Width;
            while( Index >=LineStart  )
            {
                -- Index;
                if( Text[Index] == ' ' )
                {
                    found = true;
                    break;
                }
            }

            if( found == true )
            {
                -- Index;
            }
            else
            {
                Width = Width2;
                Index = Index2;
            }

            LineWidth = Index - LineStart + 1;

            for( i=0; i<LineWidth; i++ )
                TextArray[ i ] = Text[ LineStart+i];
            TextArray[ i ] = '\0';
                       
            Lines[ LineCount ]->NumberOfChars = LineWidth;
            Lines[ LineCount ]->Characters = new char[ LineWidth + 1 ];
            strcpy( Lines[ LineCount ]->Characters , TextArray );
            Lines[ LineCount ]->LineWidth = TextSurface->GetTextWidth( TextArray );

            ++ LineCount;
            ++ Index;
            if( Text[ Index ] == ' ' )
                ++ Index;
            LineStart = Index;
            Width = 0;
        }
        else
        { // yes, it fits
            Width += ChWidth;
            ++ Index;
        }

        continue;        
    }


    // output debug informations
/*    FILE * Datei = fopen( "out.txt" , "w" );
    fprintf( Datei , "Lines: %i\n" , LineCount );
    for( i=0; i<LineCount; i++ )
    {
        fprintf( Datei , "%i %i %s\n" , Lines[i]->NumberOfChars ,
                                        Lines[i]->LineWidth ,
                                        Lines[i]->Characters );
    }
    fclose( Datei );
*/

    LineHeight = TextSurface->GetCharacterHeight( 'A' );
    TextHeight = LineCount * ( LineHeight + LineSpacing );
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
CDXVertTextScroller::~CDXVertTextScroller()
{
    int i;

    if( Text != NULL )
        delete Text;

    for( i=0; i<LineCount; i++ )
    {
        if( Lines[ i ]->Characters != NULL )
            delete Lines[ i ]->Characters;
        if( Lines[ i ]->ExtraSpace != NULL )
            delete Lines[ i ]->ExtraSpace;
        delete Lines[ i ];
    }

    if( OwnFont == true )
	    if( TextSurface != NULL )
		    delete TextSurface;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDXVertTextScroller::Draw( CDXSurface * lpDDS )
{
    int         X , Y , YSave, XSave;
    int         Row , Width , Width2;
    char *      CharPointer , ch;
    RECT        SrcRect;
    HRESULT     Result;

    Result = DD_OK;

    switch( TextJustify )
    {
        case CDX_LEFTJUSTIFY:

            Row = 0;
            Y = IY;

            while( Y + LineHeight + LineSpacing < 0 )
            {
                Y += LineHeight + LineSpacing;
                ++ Row;
            }

            if( Row >= LineCount )
                break;

            while( Y < THeight )
            {
                X  = StartX;
                CharPointer = Lines[ Row ]->Characters;
                
                while( ( ch = *CharPointer++ ) != '\0' )
                {
                    SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
                    Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;
                    if( ch != ' ' )
                    {
                        YSave = Y;
                        Y += StartY;
                        XSave = X;
                        ClipXY( &X , &Y , &SrcRect , &ClipRect );
                        Result = lpDDS->GetDDS()->BltFast( X , Y , TextSurface->TextSurface->GetDDS() , 
                                                         &SrcRect , DDBLTFAST_WAIT );
                        Y = YSave;
                        X = XSave;
                        if( Result == DDERR_SURFACELOST )
                            TextSurface->Restore( );
                    }
                    X += Width;
                }
                Y += LineHeight + LineSpacing;
                ++ Row;
                if( Row >= LineCount )
                    break;
            }
            break;


        case CDX_RIGHTJUSTIFY:

            Row = 0;
            Y = IY;

            while( Y + LineHeight + LineSpacing < 0 )
            {
                Y += LineHeight + LineSpacing;
                ++ Row;
            }

            if( Row >= LineCount )
                break;

            while( Y < THeight )
            {
                CharPointer = Lines[ Row ]->Characters;
                Width2      = Lines[ Row ]->LineWidth;
                X  = ClipRect.right - Width2;
                
                while( ( ch = *CharPointer++ ) != '\0' )
                {
                    SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
                    Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;
                    if( ch != ' ' )
                    {
                        YSave = Y;
                        Y += StartY;
                        XSave = X;
                        ClipXY( &X , &Y , &SrcRect , &ClipRect );
                        Result = lpDDS->GetDDS()->BltFast( X , Y , TextSurface->TextSurface->GetDDS() , 
                                                         &SrcRect , DDBLTFAST_WAIT );
                        Y = YSave;
                        X = XSave;
                         if( Result == DDERR_SURFACELOST )
                            TextSurface->Restore( );
                    }
                    X += Width;
                }
                Y += LineHeight + LineSpacing;
                ++ Row;
                if( Row >= LineCount )
                    break;
            }
            break;


        case CDX_CENTERJUSTIFY:

            Row = 0;
            Y = IY;

            while( Y + LineHeight + LineSpacing < 0 )
            {
                Y += LineHeight + LineSpacing;
                ++ Row;
            }

            if( Row >= LineCount )
                break;

            while( Y < THeight )
            {
                CharPointer = Lines[ Row ]->Characters;
                Width2      = Lines[ Row ]->LineWidth;
                X  = StartX + ( ( TWidth - Width2 ) / 2 );
                
                while( ( ch = *CharPointer++ ) != '\0' )
                {
                    SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
                    Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;
                    if( ch != ' ' )
                    {
                        YSave = Y;
                        Y += StartY;
                        XSave = X;
                        ClipXY( &X , &Y , &SrcRect , &ClipRect );
                        Result = lpDDS->GetDDS()->BltFast( X , Y , TextSurface->TextSurface->GetDDS() , 
                                                         &SrcRect , DDBLTFAST_WAIT );
                        Y = YSave;
                        X = XSave;                    
                        if( Result == DDERR_SURFACELOST )
                            TextSurface->Restore( );
                    }
                    X += Width;
                }
                Y += LineHeight + LineSpacing;
                ++ Row;
                if( Row >= LineCount )
                    break;
            }
            break;

    }

    return Result;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDXVertTextScroller::DrawTrans( CDXSurface * lpDDS )
{
    int         X , Y , YSave, XSave;
    int         Row , Width , Width2;
    char *      CharPointer , ch;
    RECT        SrcRect;
    HRESULT     Result;

    Result = DD_OK;

    switch( TextJustify )
    {
        case CDX_LEFTJUSTIFY:

            Row = 0;
            Y = IY;

            while( Y + LineHeight + LineSpacing < 0 )
            {
                Y += LineHeight + LineSpacing;
                ++ Row;
            }

            if( Row >= LineCount )
                break;

            while( Y < THeight )
            {
                X  = StartX;
                CharPointer = Lines[ Row ]->Characters;
                
                while( ( ch = *CharPointer++ ) != '\0' )
                {
                    Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;
                    if( ch != ' ' )
                    {
                        SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
                        YSave = Y;
                        Y += StartY;
                        XSave = X;
                        ClipXY( &X , &Y , &SrcRect , &ClipRect );
                        Result = lpDDS->GetDDS()->BltFast( X , Y , TextSurface->TextSurface->GetDDS() , 
                                                         &SrcRect , DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY );
                        Y = YSave;
                        X = XSave;
                        if( Result == DDERR_SURFACELOST )
                            TextSurface->Restore( );
                    }
                    X += Width;
                }
                Y += LineHeight + LineSpacing;
                ++ Row;
                if( Row >= LineCount )
                    break;
            }
            break;


        case CDX_RIGHTJUSTIFY:

            Row = 0;
            Y = IY;

            while( Y + LineHeight + LineSpacing < 0 )
            {
                Y += LineHeight + LineSpacing;
                ++ Row;
            }

            if( Row >= LineCount )
                break;

            while( Y < THeight )
            {
                CharPointer = Lines[ Row ]->Characters;
                Width2      = Lines[ Row ]->LineWidth;
                X  = ClipRect.right - Width2;
                
                while( ( ch = *CharPointer++ ) != '\0' )
                {
                    Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;
                    if( ch != ' ' )
                    {
                        SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
                        YSave = Y;
                        Y += StartY;
                        XSave = X;
                        ClipXY( &X , &Y , &SrcRect , &ClipRect );
                        Result = lpDDS->GetDDS()->BltFast( X , Y , TextSurface->TextSurface->GetDDS() , 
                                                         &SrcRect , DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY );
                        Y = YSave;
                        X = XSave;
                         if( Result == DDERR_SURFACELOST )
                            TextSurface->Restore( );
                    }
                    X += Width;
                }
                Y += LineHeight + LineSpacing;
                ++ Row;
                if( Row >= LineCount )
                    break;
            }
            break;


        case CDX_CENTERJUSTIFY:

            Row = 0;
            Y = IY;

            while( Y + LineHeight + LineSpacing < 0 )
            {
                Y += LineHeight + LineSpacing;
                ++ Row;
            }

            if( Row >= LineCount )
                break;

            while( Y < THeight )
            {
                CharPointer = Lines[ Row ]->Characters;
                Width2      = Lines[ Row ]->LineWidth;
                X  = StartX + ( ( TWidth - Width2 ) / 2 );
                
                while( ( ch = *CharPointer++ ) != '\0' )
                {
                    Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;
                    if( ch != ' ' )
                    {
                        SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
                        YSave = Y;
                        Y += StartY;
                        XSave = X;
                        ClipXY( &X , &Y , &SrcRect , &ClipRect );
                        Result = lpDDS->GetDDS()->BltFast( X , Y , TextSurface->TextSurface->GetDDS() , 
                                                         &SrcRect , DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY );
                        Y = YSave;
                        X = XSave;                    
                        if( Result == DDERR_SURFACELOST )
                            TextSurface->Restore( );
                    }
                    X += Width;
                }
                Y += LineHeight + LineSpacing;
                ++ Row;
                if( Row >= LineCount )
                    break;
            }
            break;

    }

    return Result;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void CDXVertTextScroller::ScrollUp( int Count )
{
    IY -= Count;
    if( IY < -TextHeight )
	{
        IY = -TextHeight;
		BottomEnd = FALSE;
		TopEnd	  = TRUE;
	}
	else
	{
		BottomEnd = FALSE;
		TopEnd	  = FALSE;
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void CDXVertTextScroller::ScrollDown( int Count )
{
    IY += Count;
    if( IY > THeight )
	{
        IY = THeight;
		BottomEnd = TRUE;
		TopEnd	  = FALSE;
	}
	else
	{
		BottomEnd = FALSE;
		TopEnd	  = FALSE;
	}

}




//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void CDXVertTextScroller::ResetPosition( void )
{
    IY = THeight;
    //IY = 0;
	BottomEnd = FALSE;
	TopEnd = FALSE;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
inline void CDXVertTextScroller::ClipX(int *DestX, RECT *SrcRect, RECT *DestRect)
{
	// If it's partly off the right side of the screen
	if(*DestX + (SrcRect->right - SrcRect->left) > DestRect->right)
		SrcRect->right -= *DestX + (SrcRect->right-SrcRect->left) - DestRect->right;

	// Partly off the left side of the screen
	if(*DestX < DestRect->left)
	{
		SrcRect->left += DestRect->left - *DestX;
		*DestX = DestRect->left;
	}

	return;
}




//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
inline void CDXVertTextScroller::ClipXY(int *DestX, int *DestY, RECT *SrcRect, RECT *DestRect)
{
	// If it's partly off the right side of the screen
	if(*DestX + (SrcRect->right - SrcRect->left) > DestRect->right)
		SrcRect->right -= *DestX + (SrcRect->right-SrcRect->left) - DestRect->right;

	// Partly off the left side of the screen
	if(*DestX < DestRect->left)
	{
		SrcRect->left += DestRect->left - *DestX;
		*DestX = DestRect->left;
	}

	// Partly off the top of the screen
	if(*DestY < DestRect->top)
	{
		SrcRect->top += DestRect->top - *DestY;
		*DestY = DestRect->top;
	}

	// If it's partly off the bottom side of the screen
	if(*DestY + (SrcRect->bottom - SrcRect->top) > DestRect->bottom)
	SrcRect->bottom -= ((SrcRect->bottom-SrcRect->top)+*DestY) - DestRect->bottom;

	return;
}
