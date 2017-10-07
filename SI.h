// ===========================================================================
//  Project:    Space Invaders
//  File:       SI.h
//  Author: Mirza Kolakovic
//  Date:   April 6, 2001.
// ===========================================================================

// ===========================================================================
//
// File:            SI.h
//
// Description:     This file is the main header file for space invaders.
//                  
//
// ===========================================================================

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "resource.h"
#include <math.h>
#include <time.h>

//#define NDEBUG //Uncomment this for release !
#include <assert.h>

#define CDXINCLUDEALL // this define includes all headers, otherwise include one by one
#include <cdx.h>

// CDX add-ons
#include ".\CDXAddons\CDXStar\CDXStar.h"  
#include ".\CDXAddons\CDXTextScrol\CDXVertTextScroller.h"
#include ".\CDXAddons\CDXMenu\CDXMenu.h"
#include ".\CDXAddons\CDXHPC\CDXHPC.h"

// SI includes
#include "ScoreIO.h"   // Writes hi-score to a file
#include "DXVersion.h" //Checks app exit status and checks DirectX version no.
#include "Options.h"   // Write options to a file

// ===========================================================================
// Defines
// ===========================================================================
/***
        Do 1 frame (at least) every 1 milliseconds or 1000 FPS (1000(ms/s)/1(ms/Frame) = 1000 FPS )

Note: Since all game movements are based on FrameTime and calculated as Velocity*FrameTime, we have
        to force frame time to be greater than or equal to 1.  This is not important now but as the 
        processors are getting faster we want to limit frame rate to 1000FPS which is more than we will
        never need.
***/
#define MIN_FRAME_TIME  1 //milliseconds

/***
Game States
***/
#define SPLASH		0
#define INTRO		1
#define SETUP		2
#define RESET       3
#define PLAY_GAME	4
#define PAUSE       5
#define NEXT_LEVEL  6
#define GAME_OVER	7

/***
Sound
***/    
//#define THUNDER_WAV		    ".\\DAS\\Thunder.das"
#define BULLET_WAV		    ".\\DAS\\B.das"
#define STEP1_WAV		    ".\\DAS\\S1.das"
#define STEP2_WAV		    ".\\DAS\\S2.das"
#define STEP3_WAV		    ".\\DAS\\S3.das"
#define STEP4_WAV		    ".\\DAS\\S4.das"
#define SI_EXPLODE_WAV	    ".\\DAS\\ESI.das"
#define UFO_EXPLODE_WAV 	".\\DAS\\EFO.das"
#define UFO_WAV             ".\\DAS\\F.das"
#define SHIP_EXPLODE_WAV	".\\DAS\\ESP.das"
//#define BOMB_EXPLODE_WAV	".\\DAS\\EBB.das"

/***
Bitmaps
***/
//#define LOGO_BMP		".\\Bitmaps\\Logo.bmp"
#define TITLE_BMP		".\\DAB\\T.dab"
#define SI_BMP			".\\DAB\\S.dab" 
#define UFO_BMP			".\\DAB\\F.dab"
#define SHIP_BMP		".\\DAB\\P.dab"
#define BULLET_BMP		".\\DAB\\B.dab" // bomb & torpedo here too.
//#define G_OVER_BMP      ".\\DAB\\GOver.dab"

/***
Menu Items
***/
#define MENU_ITEM_HEIGHT  28	// height of each menu item and spacing between items

/***
Number of Bullets, Bombs and Torpedos
***/
#define MAX_BULLETS     50
#define MAX_BOMBS       50
#define MAX_TORPEDOS    10

/***
Invaders Stuff
***/
#define HEAD_INVADERS	 32 // Number of Head Invaders
#define ANTENA_INVADERS  32
#define CONE_INVADERS    16

/***
Some Colors
***/
#define WHITE   RGB(255, 255, 255)
#define RED     RGB(255,   0,   0)
#define GREEN   RGB(  0, 255,   0)
#define YELLOW  RGB(255, 255,   0)

DWORD WhitePixel,   // We will set these at run time using DDColorMatch()
      RedPixel,     // They will have different values regarding to color 
      ShipPixel,   // depth.(256 colors, 16 bit or 32 bit colors ) 
      YellowPixel,  // This is important for collision detection.
      ShieldPixel;

/***
Stuff
***/
#define NEXT_LEVEL_DELAY  3000 //millisec

// ===========================================================================
// Global Variables
// ===========================================================================
char        szAppName[]     = "Space Invaderz";
char        szClassName[]   = "SIWndClass";

HINSTANCE   g_hInst;			        // instance handle
HWND        g_hWnd;				        // window handle

/***
Flags for the DirectDraw object, Sounds & Debug
***/
BOOL        g_bFullScreen    = TRUE;	 // running full screen?
BOOL        g_IsAppActive    = TRUE;	 // is the app active
BOOL		TripleBuffer     = FALSE;	 // triple buffering on/off
BOOL		BackBufferInVRAM = FALSE;    // backbuffer in vram/sysram
BOOL		SoundFlag        = TRUE;

#ifdef NDEBUG
BOOL	DebugFlag = FALSE;
#endif

#ifndef NDEBUG
BOOL	DebugFlag = TRUE;
#endif

DWORD DurationOfTheFrame = 0; // in milliseconds

/***
Debug File Ptr
***/
FILE	*fdebug = 0;   

/***
Screen resolution and color depth
***/
int	ScreenWidth   = 640;
int ScreenHeight  = 480;
int ScreenBPP     =   8;

/***
Flags
***/
bool bPlayThunder;
bool bRestorePalette;

/***
Palette
***/
PALETTEENTRY pe[256]; //??????????????????????? what if more than 256 colors

/***
Game Data
***/
int Lives       = 3;
int	GameState   = 0;
int GameLevel   = 0;
DWORD Score	    = 0;
DWORD HiScore   = 0;

#define HI_SCORE_COLOR  RGB(255,255,255)   // White
#define SCORE_COLOR     RGB(255,255,  0)   // Yellow

#define STAR_DELAY   30

/***
Ship stuff
***/
#define SHIP_NOT_ACTIVE        0
#define SHIP_ACTIVE            1
#define SHIP_EXPLODE           2
#define SHIP_LAST_FRAME        8
#define SHIP_Y   ScreenHeight-35
#define SHIP_EXPLODE_DELAY   150


double ShipdX;
const double ShipVelocityX = 200.0/1000.0;// 200 pixels per second

/***
Shields
***/
#define SHIELD_HEIGHT   48
#define SHIELD_WIDTH    72
#define SHIELD_Y     (SHIP_Y - 68)
#define SHIELD_1_X    40
#define SHIELD_2_X   200
#define SHIELD_3_X   360
#define SHIELD_4_X   520
// This 2D array is a shield bitmap.  Positions within array 
// set to one will be painted green while positions set to zero
// will not be painted
BYTE Shield1[SHIELD_HEIGHT][SHIELD_WIDTH]; 
BYTE Shield2[SHIELD_HEIGHT][SHIELD_WIDTH];
BYTE Shield3[SHIELD_HEIGHT][SHIELD_WIDTH];
BYTE Shield4[SHIELD_HEIGHT][SHIELD_WIDTH];

/***
UFO Stuff
***/
double UFOdX; 
char strUFOScore[20];
int BulletsSinceLastUFO;
const double UFOVelocityX  =  100.0/1000.0; //100 pixels per second

#define UFO_X              -32   // strat drawing it here
#define UFO_Y               30
#define UFO_NOT_ACTIVE      0
#define UFO_ACTIVE          1
#define UFO_EXPLODE         2
#define UFO_EXPLODE_DELAY   400  // in millisec
#define ACTIVATE_UFO_ON     32   //We will activate ufo after 32 bullets have been fired

/***
Bullet Stuff
***/
CDXSprite * Bullets[ MAX_BULLETS ]; //Array to hold bullets
const double BulletVelocityY = 700.0/1000.0;

int BulletsToFire = 1; // 1 or 3

#define BULLET_NOT_ACTIVE	    0
#define BULLET_ACTIVE		    1
#define BULLET_EXPLODE          2
#define BULLET_EXPLODE_DELAY    100 
#define BULLET_DELAY	        500 //delay in milliseconds between fire

/***
Bomb Stuff
***/
CDXSprite * Bombs[ MAX_BOMBS ]; 

const double BombVelocityY = 70.0/1000.0; // 70 pixels per second
double BombdY;
int NoActiveBombs;
#define BOMB_NOT_ACTIVE	    0
#define BOMB_ACTIVE		    1 
#define BOMB_EXPLODE        2
#define BOMB_EXPLODE_DELAY 100

/***
Torpedo Stuff
***/
CDXSprite * Torpedos[ MAX_TORPEDOS ];

const double TorpedoVelocityY = 700.0/1000.0; // 700 pixels per second

#define TORPEDO_NOT_ACTIVE	    0
#define TORPEDO_ACTIVE		    1 

/***
SI Arrays and stuff
***/
CDXSprite * SInvaders[ HEAD_INVADERS + ANTENA_INVADERS + CONE_INVADERS ];

bool MoveDown;
int  SIStartMoveOnX,
     SIStopMoveOnX,  
     SIStopMoveOnY, 
     SIStartDrawOnX,
     SIStartDrawOnY,
     SIDrawStep,
     SINewFrame,
     SIPlayStep,    //defines what sound to play while moving Invaders
     NumberOfLiveSI;

int  SIMoveDelay;   //in milliseconds

#define SI_HEAD          0  // Sprite stuff
#define SI_ANTENA        1
#define SI_CONE          2

#define SI_ACTIVE         0 
#define SI_EXPLODE	      1
#define SI_NOT_ACTIVE	  2
#define SI_EXPLODE_DELAY  3 // transition state so we can see eplosion of the invader

int SIVelocityLeft  = -10;
int SIVelocityRight =  10;
int SIVelocityDown  =  20;

#define STEP1 0 //Sound states
#define STEP2 1
#define STEP3 2
#define STEP4 3

#define EXPLODE_DELAY          200 //in milliseconds
#define TIME_TO_WAIT_INVADERS   50 

/***
Credits
***/
#define SCROLL_DELAY 25
RECT    CreditsClipRect = { 0 , 140 , 640 , SHIELD_Y };

// ===========================================================================
// CDX Objects
// ===========================================================================

CDXScreen           * Screen        = 0;// The screen object, every program must have one
CDXSurface          * Back          = 0;// Global pointer to background surface; Used instead: Screen->GetBack()
CDXSound	        * Sound	    	= 0;// Sound object	
CDXStarField        * Stars         = 0;
CDXInput		    * Input         = 0;			
CDXBitmapFont       * CreditsFont   = 0;
CDXVertTextScroller	* CreditsScroll = 0;
CDXMenu             * MainMenu      = 0;
CDXMenu				* OptionsMenu   = 0;
CDXMenu				* CurrentMenu   = 0;
CDXTile		        * SiT			= 0;
CDXTile		        * TitleT		= 0;
CDXTile				* ShipT			= 0;
CDXTile				* UfoT			= 0;
CDXTile				* BulletT		= 0;
CDXSprite           * SI            = 0;
CDXSprite			* Ship			= 0;
CDXSprite			* UFO			= 0;
CDXSprite			* Title			= 0;
CDXSprite           * Bullet		= 0;
CDXSprite           * LivesSSprite  = 0;
CDXSoundBuffer      * Step1         = 0;
CDXSoundBuffer      * Step2         = 0;
CDXSoundBuffer      * Step3         = 0;
CDXSoundBuffer      * Step4         = 0;
CDXSoundBuffer      * BulletSound   = 0;
CDXSoundBuffer      * ExplodeSI     = 0;
CDXSoundBuffer      * UFOSound      = 0;  
CDXSoundBuffer      * ExplodeUFO    = 0; 
CDXSoundBuffer      * ExplodeShip   = 0; 
CDXHPC              * PCScrollTimer = 0; 
CDXHPC				* PCShip		= 0; 
CDXHPC              * PCStars       = 0;
CDXHPC				* PCShipExplode = 0; 
CDXHPC				* PCFireBullet	= 0;
CDXHPC              * PCBullet      = 0;
CDXHPC              * PCBulletExplode = 0;
CDXHPC              * PCSIMove      = 0;
CDXHPC				* PCSIExplosion = 0; 
CDXHPC              * PCAppTiming   = 0; 
CDXHPC              * PCUFOExplosion= 0;
CDXHPC              * PCGameOver    = 0;
CDXHPC              * PCNextLevel   = 0;
CDXHPC			    * PCFrameDuration = 0;
// ============================================================================
// Forward declarations
// ===========================================================================

void 
AdjustWinStyle( void );

void 
CenterWindow( void );

void 
StartVideoMode( void );

void 
ChangeVideoMode(void );

void 
DisplaySplash( void );

void 
GameIntro( void );

void 
PlayGame( void );

void 
DisplaySetup( void );

void 
CreateMenus( void );

void 
CheckInput( void );

void 
MoveObjects( void );

void 
CollisionDetection( void);

void 
DrawObjects( void );

void 
DisplayStatistics( void );

void 
ResetGame( void );

void 
ResetShip(void);

void 
GameOver( void );

void 
PauseGame( void );

void 
NextLevel( void );

// ============================================================================
// Global Text & Data
// ===========================================================================

static char MirzaKingText[50] = "Space Invaderz  -  by Mirza King, 2001";

static char PointsText[4][80]=
    {
    	{"            =   ?   MYSTERY"},
    	{"            =  30   POINTS "},
    	{"            =  20   POINTS "},
    	{"            =  10   POINTS "}
    };


static char *ScrollText = "Space Invaderz\n\nWritten by Mirza King.\n\n\nThanks:\nPatricia Kolakovic for supporting me and creating bitmaps.\nNedzad Kolakovic for getting me TI-99/4A and Space Invaders ROM cartridge.\nAll of you who helped testing this game.\n\nSpecial thanks to CDX community.\nwww.cdxlib.com\n\nVisit us at:\nwww.mirzaking.freeservers.com\nsinvaderz@hotmail.com\n\nCopyright (c) MirzaKing, 2001\n\n\n";
                                          


BYTE ShieldBitMap[SHIELD_HEIGHT][SHIELD_WIDTH] = { \
{0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},\
{0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},\
{0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},\
{0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},\
{0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},\
{0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0},\
{0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},\
{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1},\
{1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1} \
};









/* ========== End of file _SI_H_ =================================== */
