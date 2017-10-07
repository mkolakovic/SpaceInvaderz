//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/cdx/cdx3.0/addons/CDXMenu/CDXMENU.H,v $
// $Author: mindcry $
//
// $Log: CDXMENU.H,v $
// Revision 1.2  2000/06/30 08:33:46  mindcry
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

#define MAX_MENU_ITEMS      21 //MIRZA
#define MAX_STRING_LENGTH	80

class CDXMenu
{
public:
	CDXMenu();
	void Create(CDXSurface *pSurface, int Spacing);
	void SetTitle(const char *szTitle, COLORREF color);
	void SetWrap(BOOL bWrap);
	void AddItem(char *szString, COLORREF oncolor, COLORREF offcolor, COLORREF disabledcolor=RGB(0xaa,0xaa,0xaa));
	void EnableItem(int item, BOOL bEnable);

    // drawing related
	void DrawCentered(void);
    void Draw(void);
    void Draw(int X, int Y);

	// selection modification
    void Up(void);
    void Down(void);
    void Home(void);
    void End(void);
    int Enter(void);

	CDXSurface *GetSurface() { return m_pSurface; };
	//MIRZA
	int GetCurrentItem()					{ return m_CurrentItem; }
	void SetCurrentItem(int nNewValue)		{ m_CurrentItem = nNewValue; }
	int GetItemCount()						{ return m_ItemCount; }


	
private:
	CDXSurface *m_pSurface;

	struct MenuItem {
		char m_szString[MAX_STRING_LENGTH];
		COLORREF m_OnColor;
		COLORREF m_OffColor;
		COLORREF m_DisabledColor;
		BOOL m_Enabled;
	} m_MenuItems[MAX_MENU_ITEMS];

	int m_ItemCount;
	int m_Spacing;
	int m_CurrentItem;
	BOOL m_bWrap;

	struct Title {
		char m_szTitle[MAX_STRING_LENGTH];
		COLORREF m_color;
		BOOL m_bActive;
	} m_Title;
};
