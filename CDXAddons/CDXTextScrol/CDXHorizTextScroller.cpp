// Project Name:   [ CDXHorizTextScroller ]
// Author:         [ Ioannis Karagiorgos - karagior@pluto.fernuni-hagen.de ]
// Date:           [ 7.4.1999 ]
// Revision:       [ 1.00 ]
//////////////////////////////////////////////////////////////////////////////////

#include "CDXHorizTextScroller.h"



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
CDXHorizTextScroller::CDXHorizTextScroller( CDXScreen * Scr , RECT * ClipRectangle , 
											char * ScrollText , bool TextWrap , 
                                            CDXBitmapFont * Font )
{  
    Text          = NULL;
	TextSurface   = NULL;
    IXtoCharacter = NULL;
	Screen	      = Scr;

    Wrap          = TextWrap;


    Text = new char [strlen( ScrollText )+1 ];
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

    TWidth        = ClipRect.right - StartX;

    Create( );

    ResetPosition( );
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void CDXHorizTextScroller::Create( void )
{
    int     i , j, Width;
    int     Count;
    char    ch;
    char *  Temp;

	IX = 0;

	TextLen  = strlen( Text );
    if( TextLen == 0 )
    {
        delete Text;
        Text = new char[ 2 ];
        Text[ 0 ] = ' ';
        Text[ 1 ] = '\0';
        TextLen = 1;
    }

    // add spaces at the end of the string
    if( Wrap == true )
    {
        i     = 0;
        Count = 0;
        while( i < TWidth )
        {
            i += TextSurface->GetCharacterWidth( ' ' );
            ++ Count;
        }
        if( Count > 0 )
        {
            Temp = new char[ TextLen + 1 ];
            strcpy( Temp , Text );
            delete Text;
            TextLen += Count;
            Text = new char[ TextLen + 1];
            strcpy( Text , Temp );
            for( i=0; i<Count; i++ )
                strcat( Text , " " );
            delete Temp;
        }
    }

	TextWidth   = TextSurface->GetTextWidth( Text );
	CharsHeight = TextSurface->GetCharacterHeight( 'A' );

    // Initialize IXtoCharacterArray
    if( IXtoCharacter != NULL )
        delete IXtoCharacter;

    IXtoCharacter = new CDXHTS [TextWidth];

    Count = 0;

    for( i=0; i<TextLen; i++ )
    {
        ch = Text[ i ];
        Width = TextSurface->GetCharacterWidth( ch );
        for( j=0; j<Width; j++ )
        {
            IXtoCharacter[ Count ].PositionInText = i;
            IXtoCharacter[ Count ].Character      = ch;
            IXtoCharacter[ Count ].Offset         = j;
            ++ Count;
        }
    }
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
CDXHorizTextScroller::~CDXHorizTextScroller()
{
    if( Text != NULL )
        delete Text;

    if( OwnFont == true )
	    if( TextSurface != NULL )
		    delete TextSurface;

    if( IXtoCharacter != NULL )
        delete IXtoCharacter;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDXHorizTextScroller::Draw( CDXSurface * lpDDS )
{ 
    int         X , Y, YSave;
    int         Width;
    int         Offset;
    char *      CharPointer;
    RECT        SrcRect;
    char        ch;
    HRESULT     Result;

    if( Wrap == true )
    {
        X = StartX;
        Y = StartY;
        if( X < ClipRect.right )
        {
            ch          = IXtoCharacter[ IX ].Character;
            Offset      = IXtoCharacter[ IX ].Offset;
            CharPointer = &Text[ IXtoCharacter[ IX ].PositionInText ];
        
            SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
            Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;

            X -= Offset;

            do
            {
                YSave = Y;
                ClipXY( &X , &Y , &SrcRect , &ClipRect );
                Result = lpDDS->GetDDS()->BltFast( X , Y , TextSurface->TextSurface->GetDDS() , &SrcRect , DDBLTFAST_WAIT );
                if( Result == DDERR_SURFACELOST )
                    TextSurface->Restore( );
                X += SrcRect.right - SrcRect.left;
                Y  = YSave;
                ++CharPointer;
                ch = *CharPointer;
                if( ch == '\0' )
                {
                    CharPointer = Text;
                    ch = *CharPointer;
                }

                SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
                Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;
            } 
            while( X < ClipRect.right );
        }

    }
    else
    {
        Y  = StartY;
        X  = StartX - IX;

        CharPointer = Text;
        ch = *CharPointer;
        
        Width = TextSurface->CDXBitmapFontArray[ ch ].Width;
        while( X+Width < StartX )
        {
            X += Width;
            ++ CharPointer;
            if( *CharPointer == '\0' )
                CharPointer = Text;
            ch = *CharPointer;       
            Width = TextSurface->CDXBitmapFontArray[ ch ].Width;
        }

        while( X < ClipRect.right )
        {
            SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
            Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;

            ClipX( &X , &SrcRect , &ClipRect );
            Result = lpDDS->GetDDS()->BltFast( X , Y , TextSurface->TextSurface->GetDDS() , &SrcRect , DDBLTFAST_WAIT );
            if( Result == DDERR_SURFACELOST )
                TextSurface->Restore( );
            X += SrcRect.right - SrcRect.left;

            ++CharPointer;
            ch = *CharPointer;
            if( ch == '\0' )
            {
                break;
            }
        }
    }

    return Result;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDXHorizTextScroller::DrawTrans( CDXSurface * lpDDS )
{ 
    int         X , Y , YSave;
    int         Width;
    int         Offset;
    char *      CharPointer;
    RECT        SrcRect;
    char        ch;
    HRESULT     Result;

    if( Wrap == true )
    {
        X = StartX;
        Y = StartY;
        if( X < ClipRect.right )
        {
            ch          = IXtoCharacter[ IX ].Character;
            Offset      = IXtoCharacter[ IX ].Offset;
            CharPointer = &Text[ IXtoCharacter[ IX ].PositionInText ];
        
            SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
            Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;

            X -= Offset;

            do
            {
                if( ch != ' ' )
                {
                    YSave = Y;
                    ClipXY( &X , &Y , &SrcRect , &ClipRect );
                    Result = lpDDS->GetDDS()->BltFast( X , Y , TextSurface->TextSurface->GetDDS() , 
                                            &SrcRect , DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
                    Y = YSave;
                    if( Result == DDERR_SURFACELOST )
                        TextSurface->Restore( );
                }

                X += SrcRect.right - SrcRect.left;

                ++CharPointer;
                ch = *CharPointer;
                if( ch == '\0' )
                {
                    CharPointer = Text;
                    ch = *CharPointer;
                }

                SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
                Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;
            } 
            while( X < ClipRect.right );
        }

    }
    else
    {
        Y  = StartY;
        X  = StartX - IX;

        CharPointer = Text;
        ch = *CharPointer;
        
        Width = TextSurface->CDXBitmapFontArray[ ch ].Width;
        while( X+Width < StartX )
        {
            X += Width;
            ++ CharPointer;
            if( *CharPointer == '\0' )
                CharPointer = Text;
            ch = *CharPointer;       
            Width = TextSurface->CDXBitmapFontArray[ ch ].Width;
        }

        while( X < ClipRect.right )
        {
            SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
            Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;

            if( ch != ' ' )
            {
                ClipX( &X , &SrcRect , &ClipRect );
                Result = lpDDS->GetDDS()->BltFast( X , Y , TextSurface->TextSurface->GetDDS() , 
                                        &SrcRect , DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
                if( Result == DDERR_SURFACELOST )
                    TextSurface->Restore( );
            }

            X += SrcRect.right - SrcRect.left;

            ++CharPointer;
            ch = *CharPointer;
            if( ch == '\0' )
            {
                break;
            }
        }
    }
    
    return Result;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDXHorizTextScroller::DrawOffset( CDXSurface * lpDDS , int * OffsetArray)
{ 
    int         i;
    int         X , Y;
    int         Width, ClipWidth;
    int         Offset;
    char *      CharPointer;
    RECT        SrcRect;
    char        ch;
    HRESULT     Result;

    if( Wrap == true )
    {
        X = StartX;
        Y = StartY;
        if( X < ClipRect.right )
        {
            ch          = IXtoCharacter[ IX ].Character;
            Offset      = IXtoCharacter[ IX ].Offset;
            CharPointer = &Text[ IXtoCharacter[ IX ].PositionInText ];
        
            SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
            Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;

            X -= Offset;

            do
            {
                ClipX( &X , &SrcRect , &ClipRect );
                ClipWidth = SrcRect.right - SrcRect.left;
                for( i=ClipWidth; i>0; i-- )
                {
                    SrcRect.right = SrcRect.left + 1;
                    Result = lpDDS->GetDDS()->BltFast( X++ , Y+*OffsetArray++ , TextSurface->TextSurface->GetDDS() , 
                                                      &SrcRect , DDBLTFAST_WAIT );
                    if( Result == DDERR_SURFACELOST )
                        TextSurface->Restore( );
                    SrcRect.left++;
                }

                ++CharPointer;
                ch = *CharPointer;
                if( ch == '\0' )
                {
                    CharPointer = Text;
                    ch = *CharPointer;
                }

                SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
                Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;
            } 
            while( X < ClipRect.right );
        }

    }
    else
    {
        Y  = StartY;
        X  = StartX - IX;

        CharPointer = Text;
        ch = *CharPointer;
        
        Width = TextSurface->CDXBitmapFontArray[ ch ].Width;

        while( X+Width < StartX )
        {
            X           += Width;
            ++ CharPointer;
            if( *CharPointer == '\0' )
                CharPointer = Text;
            ch = *CharPointer;       
            Width = TextSurface->CDXBitmapFontArray[ ch ].Width;
        }

        if( X >= 0 )
            OffsetArray += X;


        while( X < ClipRect.right )
        {
            SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
            Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;

            ClipX( &X , &SrcRect , &ClipRect );
            ClipWidth = SrcRect.right - SrcRect.left;
            for( i=ClipWidth; i>0; i-- )
            {
                SrcRect.right = SrcRect.left + 1;
                Result = lpDDS->GetDDS()->BltFast( X++ , Y+*OffsetArray++ , TextSurface->TextSurface->GetDDS() , 
                                                  &SrcRect , DDBLTFAST_WAIT );
                if( Result == DDERR_SURFACELOST )
                    TextSurface->Restore( );
                SrcRect.left++;
            }

            ++CharPointer;
            ch = *CharPointer;
            if( ch == '\0' )
            {
                break;
            }
        }
    }

    return Result;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDXHorizTextScroller::DrawOffsetTrans( CDXSurface * lpDDS , int * OffsetArray)
{ 
    int         i;
    int         X , Y;
    int         Width, ClipWidth;
    int         Offset;
    char *      CharPointer;
    RECT        SrcRect;
    char        ch;
    HRESULT     Result;

    if( Wrap == true )
    {
        X = StartX;
        Y = StartY;
        if( X < ClipRect.right )
        {
            ch          = IXtoCharacter[ IX ].Character;
            Offset      = IXtoCharacter[ IX ].Offset;
            CharPointer = &Text[ IXtoCharacter[ IX ].PositionInText ];
        
            SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
            Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;

            X -= Offset;

            do
            {
                ClipX( &X , &SrcRect , &ClipRect );
                ClipWidth = SrcRect.right - SrcRect.left;
                for( i=ClipWidth; i>0; i-- )
                {
                    SrcRect.right = SrcRect.left + 1;
                    Result = lpDDS->GetDDS()->BltFast( X++ , Y+*OffsetArray++ , TextSurface->TextSurface->GetDDS() , 
                                                      &SrcRect , DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY );
                    if( Result == DDERR_SURFACELOST )
                        TextSurface->Restore( );
                    SrcRect.left++;
                }

                ++CharPointer;
                ch = *CharPointer;
                if( ch == '\0' )
                {
                    CharPointer = Text;
                    ch = *CharPointer;
                }

                SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
                Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;
            } 
            while( X < ClipRect.right );
        }

    }
    else
    {
        Y  = StartY;
        X  = StartX - IX;

        CharPointer = Text;
        ch = *CharPointer;
        
        Width = TextSurface->CDXBitmapFontArray[ ch ].Width;

        while( X+Width < StartX )
        {
            X           += Width;
            ++ CharPointer;
            if( *CharPointer == '\0' )
                CharPointer = Text;
            ch = *CharPointer;       
            Width = TextSurface->CDXBitmapFontArray[ ch ].Width;
        }

        if( X >= 0 )
            OffsetArray += X;


        while( X < ClipRect.right )
        {
            SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
            Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;

            ClipX( &X , &SrcRect , &ClipRect );
            ClipWidth = SrcRect.right - SrcRect.left;
            for( i=ClipWidth; i>0; i-- )
            {
                SrcRect.right = SrcRect.left + 1;
                Result = lpDDS->GetDDS()->BltFast( X++ , Y+*OffsetArray++ , TextSurface->TextSurface->GetDDS() , &SrcRect , 
                                                  DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY );
                if( Result == DDERR_SURFACELOST )
                    TextSurface->Restore( );
                SrcRect.left++;
            }

            ++CharPointer;
            ch = *CharPointer;
            if( ch == '\0' )
            {
                break;
            }
        }
    }

    return Result;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDXHorizTextScroller::DrawStretch( CDXSurface * lpDDS , int * StretchArray)
{ 
    int         i;
    int         X , Y;
    int         Height , Width, ClipWidth;
    int         Offset;
    char *      CharPointer;
    RECT        SrcRect, DstRect;
    char        ch;
    HRESULT     Result;

    if( Wrap == true )
    {
        X = StartX;
        Y = StartY;
        if( X < ClipRect.right )
        {
            ch          = IXtoCharacter[ IX ].Character;
            Offset      = IXtoCharacter[ IX ].Offset;
            CharPointer = &Text[ IXtoCharacter[ IX ].PositionInText ];
        
            SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
            Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;
            Height  = TextSurface->CDXBitmapFontArray[ ch ].Height;

            X -= Offset;

            do
            {
                ClipX( &X , &SrcRect , &ClipRect );
                ClipWidth = SrcRect.right - SrcRect.left;
                for( i=ClipWidth; i>0; i-- )
                {
                    SrcRect.right = SrcRect.left + 1;
                    DstRect.left    = X;
                    DstRect.right   = X+1;
                    ++ X;
                    DstRect.top     = Y;
                    DstRect.bottom  = Y + Height + *StretchArray++;

                    Result = lpDDS->GetDDS()->Blt( &DstRect , TextSurface->TextSurface->GetDDS() , 
                                                  &SrcRect , DDBLT_WAIT , NULL );
                    if( Result == DDERR_SURFACELOST )
                        TextSurface->Restore( );
                    SrcRect.left++;
                }

                ++CharPointer;
                ch = *CharPointer;
                if( ch == '\0' )
                {
                    CharPointer = Text;
                    ch = *CharPointer;
                }

                SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
                Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;
                Height  = TextSurface->CDXBitmapFontArray[ ch ].Height;
            } 
            while( X < ClipRect.right );
        }

    }
    else
    {
        Y  = StartY;
        X  = StartX - IX;

        CharPointer = Text;
        ch = *CharPointer;
        
        Width = TextSurface->CDXBitmapFontArray[ ch ].Width;

        while( X+Width < StartX )
        {
            X           += Width;
            ++ CharPointer;
            if( *CharPointer == '\0' )
                CharPointer = Text;
            ch = *CharPointer;       
            Width = TextSurface->CDXBitmapFontArray[ ch ].Width;
        }

        if( X >= 0 )
            StretchArray += X;


        while( X < ClipRect.right )
        {
            SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
            Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;
            Height  = TextSurface->CDXBitmapFontArray[ ch ].Height;

            ClipX( &X , &SrcRect , &ClipRect );
            ClipWidth = SrcRect.right - SrcRect.left;
            for( i=ClipWidth; i>0; i-- )
            {
                SrcRect.right = SrcRect.left + 1;
                DstRect.left    = X;
                DstRect.right   = X+1;
                ++ X;
                DstRect.top     = Y;
                DstRect.bottom  = Y + Height + *StretchArray++;

                Result = lpDDS->GetDDS()->Blt( &DstRect , TextSurface->TextSurface->GetDDS() , 
                                              &SrcRect , DDBLT_WAIT , NULL );
                if( Result == DDERR_SURFACELOST )
                    TextSurface->Restore( );
                SrcRect.left++;
            }

            ++CharPointer;
            ch = *CharPointer;
            if( ch == '\0' )
            {
                break;
            }
        }
    }
    return Result;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDXHorizTextScroller::DrawStretchTrans( CDXSurface * lpDDS , int * StretchArray)
{ 
    int         i;
    int         X , Y;
    int         Height , Width, ClipWidth;
    int         Offset;
    char *      CharPointer;
    RECT        SrcRect, DstRect;
    char        ch;
    HRESULT     Result;

    if( Wrap == true )
    {
        X = StartX;
        Y = StartY;
        if( X < ClipRect.right )
        {
            ch          = IXtoCharacter[ IX ].Character;
            Offset      = IXtoCharacter[ IX ].Offset;
            CharPointer = &Text[ IXtoCharacter[ IX ].PositionInText ];
        
            SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
            Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;
            Height  = TextSurface->CDXBitmapFontArray[ ch ].Height;

            X -= Offset;

            do
            {
                ClipX( &X , &SrcRect , &ClipRect );
                ClipWidth = SrcRect.right - SrcRect.left;
                for( i=ClipWidth; i>0; i-- )
                {
                    SrcRect.right = SrcRect.left + 1;
                    DstRect.left    = X;
                    DstRect.right   = X+1;
                    ++ X;
                    DstRect.top     = Y;
                    DstRect.bottom  = Y + Height + *StretchArray++;

                    Result = lpDDS->GetDDS()->Blt( &DstRect , TextSurface->TextSurface->GetDDS() , 
                                                  &SrcRect , DDBLT_WAIT | DDBLT_KEYSRC , NULL );
                    if( Result == DDERR_SURFACELOST )
                        TextSurface->Restore( );
                    SrcRect.left++;
                }

                ++CharPointer;
                ch = *CharPointer;
                if( ch == '\0' )
                {
                    CharPointer = Text;
                    ch = *CharPointer;
                }

                SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
                Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;
                Height  = TextSurface->CDXBitmapFontArray[ ch ].Height;
            } 
            while( X < ClipRect.right );
        }

    }
    else
    {
        Y  = StartY;
        X  = StartX - IX;

        CharPointer = Text;
        ch = *CharPointer;
        
        Width = TextSurface->CDXBitmapFontArray[ ch ].Width;

        while( X+Width < StartX )
        {
            X           += Width;
            ++ CharPointer;
            if( *CharPointer == '\0' )
                CharPointer = Text;
            ch = *CharPointer;       
            Width = TextSurface->CDXBitmapFontArray[ ch ].Width;
        }

        if( X >= 0 )
            StretchArray += X;


        while( X < ClipRect.right )
        {
            SrcRect = TextSurface->CDXBitmapFontArray[ ch ].Rectangle;
            Width   = TextSurface->CDXBitmapFontArray[ ch ].Width;
            Height  = TextSurface->CDXBitmapFontArray[ ch ].Height;

            ClipX( &X , &SrcRect , &ClipRect );
            ClipWidth = SrcRect.right - SrcRect.left;
            for( i=ClipWidth; i>0; i-- )
            {
                SrcRect.right = SrcRect.left + 1;
                DstRect.left    = X;
                DstRect.right   = X+1;
                ++ X;
                DstRect.top     = Y;
                DstRect.bottom  = Y + Height + *StretchArray++;

                Result = lpDDS->GetDDS()->Blt( &DstRect , TextSurface->TextSurface->GetDDS() , 
                                              &SrcRect , DDBLT_WAIT | DDBLT_KEYSRC , NULL );
                if( Result == DDERR_SURFACELOST )
                    TextSurface->Restore( );
                SrcRect.left++;
            }

            ++CharPointer;
            ch = *CharPointer;
            if( ch == '\0' )
            {
                break;
            }
        }
    }
    return Result;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void CDXHorizTextScroller::ScrollLeft( int Count )
{
    if( Wrap == true )
    {
        IX += Count;
        if( IX >= TextWidth )
            IX -= TextWidth;
    }
    else
    {
        IX += Count;
        if( IX > TextWidth )
		{
            IX       = TextWidth;
			LeftEnd  = TRUE;
			RightEnd = FALSE;
		}
		else
		{
			LeftEnd  = FALSE;
			RightEnd = FALSE;
		}
    }
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void CDXHorizTextScroller::ScrollRight( int Count )
{
    if( Wrap == true )
    {
        IX -= Count;
        if( IX <= -1 )
            IX = TextWidth + IX;
    }
    else
    {
        IX -= Count;
        if( IX <= -TWidth )
		{
            IX = -TWidth;
			LeftEnd  = FALSE;
			RightEnd = TRUE;
		}
		else
		{
			LeftEnd  = FALSE;
			RightEnd = FALSE;
		}
    }
}




//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void CDXHorizTextScroller::ResetPosition( void )
{
    if( Wrap == true )
        IX = TextWidth-TWidth;
    else
        IX = -TWidth;

	LeftEnd  = FALSE;
	RightEnd = TRUE;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
inline void CDXHorizTextScroller::ClipX(int *DestX, RECT *SrcRect, RECT *DestRect)
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








inline void CDXHorizTextScroller::ClipXY(int *DestX, int *DestY, RECT *SrcRect, RECT *DestRect)
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
