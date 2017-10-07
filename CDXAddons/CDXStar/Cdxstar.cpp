//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/cdx/cdx3.0/examples/vc/ex9/CDXSTAR.CPP,v $
// $Author: istan $
//
// $Log: CDXSTAR.CPP,v $
// Revision 1.1  2001/02/17 18:19:30  istan
// Added to the repository
//
// Revision 1.1.1.1  1999/05/04 23:59:23  bsimser
// Imported addons
//
// $Revision: 1.1 $
//////////////////////////////////////////////////////////////////////////////////
#include "CDX.h"
#include "CDXStar.h"
#include <time.h>

//////////////////////////////////////////////////////////////////////////////////
// CDXStarField Constructor
//////////////////////////////////////////////////////////////////////////////////
CDXStarField::CDXStarField(CDXScreen *pScreen)
{
	m_Screen = pScreen;
	m_NumStars = 100;
	m_StarVelocity = 1;
	m_Top = 0;
	m_Left = 0;
	m_Width = m_Screen->GetWidth();
	m_Right = m_Width;
	m_Height = m_Screen->GetHeight();
	m_Bottom = m_Height-32;
	m_CenterX = m_Width / 2;
	m_CenterY = m_Height / 2;
	m_StarMode = THREED;
	m_Towards = TRUE;
	m_Colored = TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// CDXStarField Destructor
//////////////////////////////////////////////////////////////////////////////////
CDXStarField::~CDXStarField()
{
}

//////////////////////////////////////////////////////////////////////////////////
// CDXStarField Create
//////////////////////////////////////////////////////////////////////////////////
void CDXStarField::Create()
{
	CreateStars();
	CalcPosition();
}

//////////////////////////////////////////////////////////////////////////////////
// CDXStarField Update
//////////////////////////////////////////////////////////////////////////////////
void CDXStarField::Update(BOOL bErase)
{
	if(bErase)
		ClearStars();
	CalcPosition();
}

//////////////////////////////////////////////////////////////////////////////////
// CDXStarField CreateStars
//////////////////////////////////////////////////////////////////////////////////
void CDXStarField::CreateStars()
{
	srand((unsigned)time(NULL));
	for(int i=0; i<m_NumStars; i++)
	{

			m_Stars[i].m_X = rand()%m_Width - m_Width / 2;
			m_Stars[i].m_Y = rand()%m_Height - m_Height / 2;
			m_Stars[i].m_Z = rand()%255;
			m_Stars[i].m_Color = 255;

	}
}

//////////////////////////////////////////////////////////////////////////////////
// CDXStarField CalcPosition
//////////////////////////////////////////////////////////////////////////////////
void CDXStarField::CalcPosition()
{
	int newx, newy;

	m_Screen->GetBack()->Lock();

	for(int i=0; i<m_NumStars; i++)
	{

			if((m_Stars[i].m_Z > 0) && (m_Stars[i].m_Z < 256))
			{
				newx = ((m_Stars[i].m_X << 7) / m_Stars[i].m_Z) + m_CenterX;
				newy = ((m_Stars[i].m_Y << 7) / m_Stars[i].m_Z) + m_CenterY;
				if((newx > m_Left) && (newx < m_Right) && 
					(newy > m_Top) && (newy < m_Bottom))
				{
				/*	if(!m_Colored)
					{
						if(m_Flashing)
							m_Stars[i].m_Color = rand()%255;
						else
						{
							if(m_Stars[i].m_Z >= 0 && m_Stars[i].m_Z < 50)
								m_Stars[i].m_Color = 244;
							else if(m_Stars[i].m_Z >= 50 && m_Stars[i].m_Z < 100)
								m_Stars[i].m_Color = 243;
							else if(m_Stars[i].m_Z >= 100 && m_Stars[i].m_Z < 150)
								m_Stars[i].m_Color = 242;
							else if(m_Stars[i].m_Z >= 150 && m_Stars[i].m_Z < 200)
								m_Stars[i].m_Color = 241;
							else
								m_Stars[i].m_Color = 240;
						}
					}
					
					*/
						
					m_Screen->GetBack()->PutPixel(newx, newy, White);//m_Stars[i].m_Color);
					m_Stars[i].m_OldX = newx;
					m_Stars[i].m_OldY = newy;
					if(m_Towards)
						m_Stars[i].m_Z -= m_StarVelocity;
					else
						m_Stars[i].m_Z += m_StarVelocity;
				}
				else
				{
					m_Stars[i].m_X = rand()%m_Width - m_Width / 2;
					m_Stars[i].m_Y = rand()%m_Height - m_Height / 2;
					m_Stars[i].m_Color = rand()%255;
					if(m_Towards)
						m_Stars[i].m_Z = 255;
					else
						m_Stars[i].m_Z = rand()%255;
				}
			}
/*			else
			{
				m_Stars[i].m_X = rand()%m_Width - m_Width / 2;
				m_Stars[i].m_Y = rand()%m_Height - m_Height / 2;
				m_Stars[i].m_Color = rand()%255;
				if(m_Towards)
					m_Stars[i].m_Z = 255;
				else
					m_Stars[i].m_Z = rand()%255;
			}
			
*/

	
	}

	m_Screen->GetBack()->UnLock();
}

//////////////////////////////////////////////////////////////////////////////////
// CDXStarField ClearStars
//////////////////////////////////////////////////////////////////////////////////
void CDXStarField::ClearStars()
{
	m_Screen->GetBack()->Fill(0);
}

//////////////////////////////////////////////////////////////////////////////////
// CDXStarField SetNumberofStars
//////////////////////////////////////////////////////////////////////////////////
void CDXStarField::SetNumberofStars(int iStars)
{
	if(iStars != m_NumStars)
	{
		m_NumStars = iStars;
		if(m_NumStars > MAXSTARS)
			m_NumStars = MAXSTARS;
		if(m_NumStars < 0)
			m_NumStars = 0;
	}
}

//mirza
void CDXStarField::AllWhite( void )
{
	for(int i=0; i<m_NumStars; i++)
	{
		m_Stars[i].m_Color = White;
	}
}

void CDXStarField::DrawStars( void )
{
    m_Screen->GetBack()->Lock();
	for(int i=0; i<m_NumStars; i++)
	{
	    m_Screen->GetBack()->PutPixel(m_Stars[i].m_OldX, m_Stars[i].m_OldY, White);//m_Stars[i].m_Color);	
	}
    m_Screen->GetBack()->UnLock();
}