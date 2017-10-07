//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/cdx/cdx3.0/examples/vc/ex9/CDXSTAR.H,v $
// $Author: istan $
//
// $Log: CDXSTAR.H,v $
// Revision 1.1  2001/02/17 18:19:30  istan
// Added to the repository
//
// Revision 1.1.1.1  1999/05/04 23:59:23  bsimser
// Imported addons
//
// $Revision: 1.1 $
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// CDXStarField Class
//////////////////////////////////////////////////////////////////////////////////
#include "cdxscreen.h"
#include "cdxsurface.h"

#define MAXSTARS	100
#define White	RGB(255,255,255)
enum m_Mode { HORIZONTAL, VERTICAL, THREED };

class CDXStarFieldRecord
{
public:
	int m_X;
	int m_Y;
	int m_Z;
	int m_OldX;
	int m_OldY;
	int m_Color;
};

class CDXStarField
{
public:
	CDXStarField(CDXScreen*);
	~CDXStarField();

	void Create();
	void Update(BOOL bErase = TRUE);
	void CreateStars();
	void CalcPosition();
	void ClearStars();
	void SetNumberofStars(int);
	//mirza
	void AllWhite( void );
    void DrawStars( void );

public:
	CDXScreen* m_Screen;
	CDXStarFieldRecord m_Stars[MAXSTARS];
    
	int m_NumStars;
	int m_StarVelocity;
	int m_Width;
	int m_Height;
	int m_LWidth;
	int m_LHeight;
	int m_CenterX;
	int m_CenterY;
	int m_Left;
	int m_Right;
	int m_Top;
	int m_Bottom;
	
	m_Mode m_StarMode;

	BOOL m_Towards;
	BOOL m_Flashing;
	BOOL m_Colored;
};
