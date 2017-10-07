// Project Name:   [ CDXVertTextScroller ]
// Author:         [ Ioannis Karagiorgos - karagior@pluto.fernuni-hagen.de ]
// Date:           [ 7.4.1999 ]
// Revision:       [ 1.00 ]
//////////////////////////////////////////////////////////////////////////////////

#ifndef __CDXVERTTEXTSCROLLER__
#define __CDXVERTTEXTSCROLLER__

#define CDXINCLUDEALL
#include <cdx.h>
#include "CDXBitmapFont.h"

#ifndef CDX_CENTERJUSTIFY
#define CDX_CENTERJUSTIFY 1
#endif
#ifndef CDX_LEFTJUSTIFY
#define CDX_LEFTJUSTIFY 2
#endif
#ifndef CDX_RIGHTJUSTIFY
#define CDX_RIGHTJUSTIFY 3
#endif

#define CDXVERTSCROLLERMAXLINES 500

class CDXVertTextScroller
{
    struct CDXVertScrollerStruct
    {
        int     NumberOfChars;
        int     LineWidth;
        char  * Characters;
        int   * ExtraSpace;
    };

    public:

	CDXBitmapFont       *	TextSurface;
	CDXScreen           *	Screen;
    char                *   Text;
    CDXVertScrollerStruct * Lines[ CDXVERTSCROLLERMAXLINES ];
    int                 LineCount;
    int                 LineHeight;
    int                 LineSpacing;
    int                 IY;
    RECT                ClipRect;
    int                 TextJustify;
    int                 StartX , StartY;
	int				    TWidth, THeight;
    int                 TextHeight;
    bool                OwnFont;
	BOOL				BottomEnd, TopEnd;

	CDXVertTextScroller( CDXScreen * Scr , RECT * ClipRectangle , 
                          char * ScrollText , int TextJustify = CDX_LEFTJUSTIFY, 
                          int LineSpace=3 , CDXBitmapFont * Font = NULL );

    void Create( void );
	~CDXVertTextScroller();

	HRESULT Draw( CDXSurface * lpDDS );
    HRESULT DrawTrans( CDXSurface * lpDDS );

	void ScrollUp( int Count = 1 );
	void ScrollDown( int Count = 1 );
    void ResetPosition( );
	BOOL IsBottomEnd( void ) { return BottomEnd; };
	BOOL IsTopEnd( void )    { return TopEnd; };

    private:
    inline void ClipX(int *DestX, RECT *SrcRect, RECT *DestRect);
	inline void ClipXY(int *DestX, int *DestY, RECT *SrcRect, RECT *DestRect);
};

#endif