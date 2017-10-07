//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/cdx/cdx3.0/addons/CDXMenu/CDXMENU.CPP,v $
// $Author: mindcry $
//
// $Log: CDXMENU.CPP,v $
// Revision 1.2  2000/06/30 08:31:46  mindcry
// Some minor fixes from Mike
//
// Revision 1.1.1.1  2000/04/22 16:50:12  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 1.1.1.1  1999/05/04 23:59:23  bsimser
// Imported addons
//
// $Revision: 1.2 $
//////////////////////////////////////////////////////////////////////////////////
#include "CDX.h"
#include "CDXMenu.h"

//////////////////////////////////////////////////////////////////////////////////
// CDXMenu Default Constructor
//////////////////////////////////////////////////////////////////////////////////
CDXMenu::CDXMenu()
{
}

//////////////////////////////////////////////////////////////////////////////////
// CDXMenu Constructor
//////////////////////////////////////////////////////////////////////////////////
void CDXMenu::Create(CDXSurface *pSurface, int Spacing)
{
	m_Spacing = Spacing;
	m_ItemCount = 0;
	m_CurrentItem = 0;
	m_pSurface = pSurface;
	m_bWrap = TRUE;
	ZeroMemory(&m_Title, sizeof(m_Title));
}

//////////////////////////////////////////////////////////////////////////////////
// Sets the wrapping capabilities of the menu
//////////////////////////////////////////////////////////////////////////////////
void CDXMenu::SetWrap(BOOL bWrap)
{
	m_bWrap = bWrap;
}

//////////////////////////////////////////////////////////////////////////////////
// Sets the title used by the menu (optional)
//////////////////////////////////////////////////////////////////////////////////
void CDXMenu::SetTitle(const char *szTitle, COLORREF color)
{
	strcpy(m_Title.m_szTitle, szTitle);
	m_Title.m_color = color;
	m_Title.m_bActive = TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Adds an item to the menu
//////////////////////////////////////////////////////////////////////////////////
void CDXMenu::AddItem(char *szString, COLORREF oncolor, COLORREF offcolor, COLORREF disabledcolor)
{
	strcpy(m_MenuItems[m_ItemCount].m_szString, szString);
	m_MenuItems[m_ItemCount].m_OnColor = oncolor;
	m_MenuItems[m_ItemCount].m_OffColor = offcolor;
	m_MenuItems[m_ItemCount].m_DisabledColor = disabledcolor;
	m_MenuItems[m_ItemCount].m_Enabled = TRUE;
	m_ItemCount++;
}

//////////////////////////////////////////////////////////////////////////////////
// Returns the current menu item
//////////////////////////////////////////////////////////////////////////////////
int CDXMenu::Enter()
{
	return m_CurrentItem;
}

//////////////////////////////////////////////////////////////////////////////////
// Enables / Disables a menu item
//////////////////////////////////////////////////////////////////////////////////
void CDXMenu::EnableItem(int item, BOOL bEnable)
{
	m_MenuItems[item].m_Enabled = bEnable;
}

//////////////////////////////////////////////////////////////////////////////////
// Sets the current item to the first on in the menu
//////////////////////////////////////////////////////////////////////////////////
void CDXMenu::Home()
{
	m_CurrentItem = 0;
}

//////////////////////////////////////////////////////////////////////////////////
// Moves up one menu option. If wrapping is enabled, wraps to the last item
//////////////////////////////////////////////////////////////////////////////////
void CDXMenu::Up()
{
	m_CurrentItem--;
	if(m_CurrentItem < 0) {
		if(m_bWrap) {
			End();
		}
		else {
			Home();
		}
	}
	if (!m_MenuItems[m_CurrentItem].m_Enabled) Up();
}

//////////////////////////////////////////////////////////////////////////////////
// Moves down one menu option. If wrapping is enabled, wraps to the first item
//////////////////////////////////////////////////////////////////////////////////
void CDXMenu::Down()
{
	m_CurrentItem++;
	if(m_CurrentItem > (m_ItemCount-1)) {
		if(m_bWrap) {
			Home();
		}
		else {
			End();
		}
	}
	if (!m_MenuItems[m_CurrentItem].m_Enabled) Down();
}

//////////////////////////////////////////////////////////////////////////////////
// Sets the last menu item as the current one
//////////////////////////////////////////////////////////////////////////////////
void CDXMenu::End()
{
	m_CurrentItem = m_ItemCount - 1;
}

//////////////////////////////////////////////////////////////////////////////////
// Draws the menu to the surface specified in the constructor. This method
// draws the menu centered on the screen and centers all the items in
// the menu.
//////////////////////////////////////////////////////////////////////////////////
void CDXMenu::DrawCentered(void)
{
    SIZE sz;
    COLORREF col;
    int X, Y, i;
	HDC hdc;

    hdc = m_pSurface->GetDC();
    m_pSurface->SetFont();

    // get the height and width of the title in the current font
    GetTextExtentPoint32(hdc, m_Title.m_szTitle,
        lstrlen(m_Title.m_szTitle), &sz);

    // calc the full height of the menu
    int mh = (m_Spacing * (m_ItemCount));

    // if the title is active add it to the height
    if(m_Title.m_bActive) {
        mh+=m_Spacing;
    }

    // calc the vertical starting point
    Y = (m_pSurface->GetHeight() - mh) / 2;

    // draw the title if its active
    if(m_Title.m_bActive) {
        // determine the X drawing for the title and draw it
        X = (m_pSurface->GetWidth() - sz.cx) / 2;
        m_pSurface->TextXY(X, Y, m_Title.m_color, m_Title.m_szTitle);
    }

    // draw all the items
    for(i=0; i<m_ItemCount; i++) {
        // get the width of the current string
        GetTextExtentPoint32(hdc, m_MenuItems[i].m_szString,
            lstrlen(m_MenuItems[i].m_szString), &sz);
        // calc its horizontal position
        X = (m_pSurface->GetWidth() - sz.cx) / 2;

		col = (i == m_CurrentItem) ? m_MenuItems[i].m_OnColor : m_MenuItems[i].m_OffColor;
		if (!m_MenuItems[i].m_Enabled) col = m_MenuItems[i].m_DisabledColor;
		if(m_Title.m_bActive) {
			m_pSurface->TextXY(X, Y+((i+1)*m_Spacing), col, m_MenuItems[i].m_szString);
		}
		else {
			m_pSurface->TextXY(X, Y+((i+1)*m_Spacing), col, m_MenuItems[i].m_szString);
		}
	}

	m_pSurface->ReleaseDC();
}

//////////////////////////////////////////////////////////////////////////////////
// Draws the menu to the surface specified in the constructor. This
// method figures out the X and Y values and places the menu in the
// center of the screen
//////////////////////////////////////////////////////////////////////////////////
void CDXMenu::Draw(void)
{
    SIZE sz;
    int dwWidest, i;
	HDC hdc;

    hdc = m_pSurface->GetDC();
    m_pSurface->SetFont();

    // get the height and width of the title in the current font
    GetTextExtentPoint32(hdc, m_Title.m_szTitle,
        lstrlen(m_Title.m_szTitle), &sz);

    // calc the full height of the menu
    int mh = (m_Spacing * (m_ItemCount));

    // if the title is active, add it to the height also
    if(m_Title.m_bActive) {
        dwWidest = sz.cx;
        mh+=m_Spacing;
    }
    else {
        dwWidest = 0;
    }

    // go through each item and figure out the widest one
    for(i=0; i<m_ItemCount; i++) {
        // get the width of the current string
        GetTextExtentPoint32(hdc, m_MenuItems[i].m_szString,
            lstrlen(m_MenuItems[i].m_szString), &sz);
        dwWidest = max(sz.cx, dwWidest);
    }

    // calc the horizontal and vertical starting point
    int X = (m_pSurface->GetWidth() - dwWidest) / 2;
    int Y = (m_pSurface->GetHeight() - mh) / 2;

    // release the handle and draw the menu
    m_pSurface->ReleaseDC();

    Draw(X, Y);
}

//////////////////////////////////////////////////////////////////////////////////
// Draws the menu to the surface specified in the constructor. This
// uses the X and Y parameters passed to place the menu.
//////////////////////////////////////////////////////////////////////////////////
void CDXMenu::Draw(int X, int Y)
{
	COLORREF col;
    int i;

	m_pSurface->GetDC();
	m_pSurface->SetFont();

	if(m_Title.m_bActive) {
		m_pSurface->TextXY(X, Y, m_Title.m_color, m_Title.m_szTitle);
	}

    for(i=0; i<m_ItemCount; i++) {
		col = (i == m_CurrentItem) ? m_MenuItems[i].m_OnColor : m_MenuItems[i].m_OffColor;
		if (!m_MenuItems[i].m_Enabled) col = m_MenuItems[i].m_DisabledColor;
		if(m_Title.m_bActive) {
			m_pSurface->TextXY(X, Y+((i+1)*m_Spacing), col, m_MenuItems[i].m_szString);
		}
		else {
			m_pSurface->TextXY(X, Y+(i+1*m_Spacing), col, m_MenuItems[i].m_szString);
		}
	}

	m_pSurface->ReleaseDC();
}
