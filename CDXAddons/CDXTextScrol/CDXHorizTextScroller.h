// Project Name:   [ CDXHorizTextScroller ]
// Author:         [ Ioannis Karagiorgos - karagior@pluto.fernuni-hagen.de ]
// Date:           [ 7.4.1999 ]
// Revision:       [ 1.00 ]
//////////////////////////////////////////////////////////////////////////////////
#ifndef __CDXHORIZTEXTSCROLLER__
#define __CDXHORIZTEXTSCROLLER__

#define CDXINCLUDEALL
#include <cdx.h>
#include "CDXBitmapFont.h"


class CDXHorizTextScroller
{
    typedef struct
    {
        WORD    PositionInText;
        char    Character;
        char    Offset;
    } CDXHTS;

    public:

	int				CharsHeight;
    int             StartX , StartY;
	int				TWidth;
	int				IX;
	char *			Text;
	int				TextWidth;
	int				TextLen;
    int             Position;

    CDXHTS *        IXtoCharacter;

	CDXBitmapFont   *	TextSurface;
	CDXScreen       *	Screen;

    RECT            ClipRect;

    bool            OwnFont;
    bool            Wrap; 

	BOOL			LeftEnd, RightEnd;


	CDXHorizTextScroller( CDXScreen * Scr , RECT * ClipRectangle , 
                          char * ScrollText , bool TextWrap , CDXBitmapFont * Font = NULL );

    void Create( void );
	~CDXHorizTextScroller();

	HRESULT Draw( CDXSurface * lpDDS );
    HRESULT DrawOffset( CDXSurface * lpDDS , int * OffsetArray);
    HRESULT DrawStretch( CDXSurface * lpDDS , int * StretchArray);
    HRESULT DrawTrans( CDXSurface * lpDDS );
    HRESULT DrawOffsetTrans( CDXSurface * lpDDS , int * OffsetArray);
    HRESULT DrawStretchTrans( CDXSurface * lpDDS , int * StretchArray);

	void ScrollLeft( int Count = 1 );
	void ScrollRight( int Count = 1 );
    void ResetPosition( );

	int GetPos( void ) { return IX; }
	BOOL IsLeftEnd( void )  { return LeftEnd; };
	BOOL IsRightEnd( void ) { return RightEnd; };


    private:
    inline void ClipX(int *DestX, RECT *SrcRect, RECT *DestRect);
    inline void ClipXY(int *DestX, int *DestY , RECT *SrcRect, RECT *DestRect);
};

#endif