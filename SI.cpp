// ===========================================================================
//  Project:    Space Invaders
//  File:       SI.cpp
//  Author: Mirza Kolakovic
//  Date:   April 6, 2001.
// ===========================================================================

// ===========================================================================
//
// File:            SI.CPP
//
// Description:     This file is the main source file for Space InvaderZ.
//                  
//
// ===========================================================================

#include "SI.h"
#include "debug.h"

// ---------------------------------------------------------------------------
//
// Name:        cdx_Init          
//
// Function:    handles initialization of the CDX objects    
//                  
//
// Parameters:      None.
//
// Return value:    BOOL.
//
// ---------------------------------------------------------------------------
BOOL cdx_Init( void )
{
    int i,j;

	/***
    Create the CDXSreen object
    ***/
	Screen = new CDXScreen();
	if (Screen==NULL)
        CDXError( NULL , "Could not create CDXScreen object" );

	/***
    Enable triple buffering ?
    ***/
	Screen->SetTripleBuffering(TripleBuffer);
	/***
    Create Back buffer in Video RAM ? (too slow)
    ***/
	Screen->CreateBackBufferInVRAM( BackBufferInVRAM );

	/***
    Start app
    ***/
    StartVideoMode();

	/***
    Open Debug file?
    ***/
    if (DebugFlag ) 
		fdebug = fopen("debug.txt","a+");
		
	/***
    Create Sound Object
    ***/
	Sound = new CDXSound;
	Sound->Create(g_hWnd);

	/***
    Create Direct Input Object
    ***/
	Input = new CDXInput();
	Input->Create(g_hInst, g_hWnd );

    /***
    Load the tiles
    ***/
	TitleT = new CDXTile();
			 if( TitleT->Create( Screen , TITLE_BMP , 350 , 150 , 1 ,  CDXMEM_SYSTEMONLY  ) == FALSE)
             {
				CDXError( Screen , "Could not load tiles from file TITLE.BMP" );
             }
    UfoT   = new CDXTile();
			 if( UfoT->Create( Screen , UFO_BMP ,       32 , 32 , 2 ,  CDXMEM_SYSTEMONLY  ) == FALSE)
             {
				CDXError( Screen , "Could not load tiles from file UFO_BMP" );
             }
    ShipT  = new CDXTile();
			 if( ShipT->Create( Screen , SHIP_BMP ,     32 , 32 , 2 ,  CDXMEM_SYSTEMONLY  ) == FALSE)// ??? ADD EXPLOSION
             {
				CDXError( Screen , "Could not load tiles from file SHIP_BMP" );
             }
    SiT    = new CDXTile();
			 if( SiT->Create( Screen , SI_BMP ,         32 , 32, 12 , CDXMEM_SYSTEMONLY  ) == FALSE)
             {
				CDXError( Screen , "Could not load tiles from file SI_BMP" );
             }
    BulletT= new CDXTile();
			 if( BulletT->Create( Screen , BULLET_BMP , 16 , 16, 3 ,   CDXMEM_SYSTEMONLY  ) == FALSE) // ??? ADD BOMB AND TORPEDO AND EXPLOSION
             {
				CDXError( Screen , "Could not load tiles from file BULLET_BMP" );
             }

	/***
    Create Sprites
    ***/

	Title = new CDXSprite();
		    Title->Create(TitleT);
			Title->SetColorKey(0); 
	UFO   = new CDXSprite();
			UFO->Create(UfoT);
			UFO->SetColorKey(0); 
	Ship  = new CDXSprite();
			Ship->Create(ShipT);
	SI    = new CDXSprite();
			SI->Create(SiT);
			SI->SetColorKey(0); 
    LivesSSprite = new CDXSprite();
            LivesSSprite->Create(ShipT);
            LivesSSprite->SetScale ( 0.5 ); // SCALE IT
            LivesSSprite->SetColorKey(0);
            LivesSSprite->SetFrame(1);

	/***
    Create bullets
    ***/
	for( i=0; i < MAX_BULLETS; i++) 
	{
		Bullets[i] = 0;
		Bullets[i] = new CDXSprite();
		Bullets[i]->Create(BulletT);
	}

	/***
    Create bombs
    ***/
	for( i=0; i < MAX_BOMBS; i++) 
	{
		Bombs[i] = 0;
		Bombs[i] = new CDXSprite();
		Bombs[i]->Create(BulletT);
	}

	/***
    Create Invaders 
    ***/
	for( j=0; j < (HEAD_INVADERS+ANTENA_INVADERS+CONE_INVADERS); j++)
	{
		SInvaders[j] = 0;
		SInvaders[j] = new CDXSprite();
		SInvaders[j]->Create( SiT );
	}

	/***
    Create torpedos
    ***/
	for( i=0; i < MAX_TORPEDOS; i++) 
	{
		Torpedos[i] = 0;
		Torpedos[i] = new CDXSprite();
		Torpedos[i]->Create(BulletT);
	}

	/***
    Load Sounds
    ***/
	BulletSound = new CDXSoundBuffer();
			BulletSound->Load( Sound,   BULLET_WAV,      5 );
	Step1 = new CDXSoundBuffer();
			Step1->Load( Sound,         STEP1_WAV,       5 );
	Step2 = new CDXSoundBuffer();
			Step2->Load( Sound,         STEP2_WAV,       5 );
	Step3 = new CDXSoundBuffer();
			Step3->Load( Sound,         STEP3_WAV,       5 );
	Step4 = new CDXSoundBuffer();
			Step4->Load( Sound,         STEP4_WAV,       5 );
    UFOSound = new CDXSoundBuffer();
            UFOSound->Load( Sound,      UFO_WAV,         5 );
	ExplodeSI = new CDXSoundBuffer();
			ExplodeSI->Load( Sound,     SI_EXPLODE_WAV,  5 );
    ExplodeUFO = new CDXSoundBuffer();
            ExplodeUFO->Load( Sound,    UFO_EXPLODE_WAV, 2 );
    ExplodeShip = new CDXSoundBuffer();
            ExplodeShip->Load( Sound,   SHIP_EXPLODE_WAV,2 );   // ?????????????????

	/***
    Create Stars
    ***/
	Stars = new CDXStarField(Screen);
			Stars->m_Towards = TRUE;
			Stars->m_Flashing = FALSE;
			Stars->m_Colored = FALSE;
			Stars->Create();

	/***
    Create Menus
    ***/
	CreateMenus();
	// Set active menu
	CurrentMenu = MainMenu;

    /***
    Create Credits Scroll Text
    ***/
    CreditsFont = new CDXBitmapFont( Screen , "Westminster" , 22 , YELLOW );
    CreditsScroll = new CDXVertTextScroller( Screen , &CreditsClipRect ,ScrollText, CDX_CENTERJUSTIFY , 4 , CreditsFont );
    
	/***
    Get default Screen Palette
    ***/                            
	Screen->GetPalette(0, 256, pe);  //????????????????????

	/***
    Create Timers
    ***/
    PCScrollTimer   = new CDXHPC();
	PCFrameDuration	= new CDXHPC();
	PCShip          = new CDXHPC();
	PCFireBullet    = new CDXHPC();
	PCBullet        = new CDXHPC();
	PCSIMove        = new CDXHPC();
	PCSIExplosion   = new CDXHPC();
    PCUFOExplosion  = new CDXHPC();
    PCBulletExplode = new CDXHPC();
    PCShipExplode   = new CDXHPC();
    PCStars         = new CDXHPC();
    PCGameOver      = new CDXHPC(); //?????????????????????
	PCNextLevel     = new CDXHPC();
	
    /***
    Start counters
    ***/
	PCShip->Start(); 
    PCStars->Start();
	PCBullet->Start();
	PCSIMove->Start();
	PCNextLevel->Start();
    PCGameOver->Start();	
    PCFireBullet->Start();
	PCSIExplosion->Start();
    PCShipExplode->Start();
    PCScrollTimer->Start();
    PCUFOExplosion->Start();
    PCFrameDuration->Start();
    PCBulletExplode->Start();

    /***
    Load HiScore from a file.
    ***/
	HiScore = ReadHighScore(); 
    
    bPlayThunder    = true; // Play Thunder       ???????????????
	bRestorePalette = true; // Show stars and restore palette

    // Seed the random-number generator with current time 
    srand( (unsigned)time( NULL ) );

	/***
    Set GameState & GO !!!!!!!!!!!
    ***/
	GameState = SPLASH;

	return TRUE;

}/* ==== End of: void cdx_Init( void )  =================================== */


// ---------------------------------------------------------------------------
//
// Name:        cdx_DeInit          
//
// Function:    handles cleanup of CDX objects   
//                  
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void cdx_DeInit( void )
{
    int i;

	if( BulletSound )
    {
        BulletSound->Stop();
        delete BulletSound;
    }
    else
    if(Step1)
    {
        Step1->Stop();
        delete Step1;
    }   
	else
    if(Step2) 
    {   
        Step2->Stop();
        delete Step2;
    }
	else   
	if(Step3)
    {
        Step3->Stop();
        delete Step3;
    }
	else
    if(Step4) 
    {
        Step4->Stop();   
        delete Step4;    
    }
    else
    if(ExplodeSI) 
    {
        ExplodeSI->Stop();   
        delete ExplodeSI;
    }
	else
    if(ExplodeUFO) 
    {
        ExplodeUFO->Stop();   
        delete ExplodeUFO;
    }
    else
    if(ExplodeShip)
    {
        ExplodeShip->Stop();  
        delete ExplodeShip;
    }
    else
    if(UFOSound) 
    {
        UFOSound->Stop();   
        delete UFOSound;
    }


	for( i=0; i < MAX_BULLETS; i++)
		delete Bullets[i];

	for( i=0; i < MAX_BOMBS; i++)
		delete Bombs[i];

	for( i=0; i < MAX_TORPEDOS; i++)
		delete Torpedos[i];

	for( i=0; i < (HEAD_INVADERS+ANTENA_INVADERS+CONE_INVADERS); i++ )
		delete SInvaders[i];
 
    SAFEDELETE( PCScrollTimer  );
    SAFEDELETE( LivesSSprite   );
	SAFEDELETE( BulletT	       );
	SAFEDELETE( Ship	       );
	SAFEDELETE( SI		       );
	SAFEDELETE( Title	       );
	SAFEDELETE( TitleT	       );
	SAFEDELETE( SiT		       );
	SAFEDELETE( ShipT	       );
	SAFEDELETE( UfoT	       );
    SAFEDELETE( PCBulletExplode);
    SAFEDELETE( PCUFOExplosion );
	SAFEDELETE( PCSIExplosion  );
	SAFEDELETE( PCSIMove       );
	SAFEDELETE( PCBullet       );
	SAFEDELETE( PCFireBullet   );
    SAFEDELETE( PCAppTiming    );
    SAFEDELETE( PCFrameDuration);
	SAFEDELETE( PCShip	       );
    SAFEDELETE( PCShipExplode  );
    SAFEDELETE( PCGameOver     );
    SAFEDELETE( PCStars        );
	SAFEDELETE( Sound	       );
	SAFEDELETE( Stars	       );
	SAFEDELETE( Input	       );
	SAFEDELETE( CreditsFont    );
	SAFEDELETE( CreditsScroll  );
	SAFEDELETE( MainMenu       );
	SAFEDELETE( OptionsMenu    );
	SAFEDELETE( Screen         );

	// Close the Debug file
    if (DebugFlag)
		fclose(fdebug);
    
    // Save Options
    WriteOptions( (((unsigned)g_bFullScreen&0xff)<<8) | ((unsigned)SoundFlag&0xff) );
	
    // Write Exir status 1 = "Success"
    WriteExitStatus( 0x1 );

}/* ==== End of: void cdx_DeInit( void )  ================================= */


// ---------------------------------------------------------------------------
//
// Name:        cdx_DoFrame        
//
// Function:    Main game loop!  Here we decide what part of the game we want 
//              to show, depending on the GameState variable.  (State Machine)
//                  
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void cdx_DoFrame()
{
	// Get background surface
    Back = Screen->GetBack();

	switch( GameState )
	{
		case SPLASH:
			DisplaySplash();
			break;

		case INTRO:
			GameIntro();
			break;

		case SETUP:
			DisplaySetup();
			break;

        case RESET:
            ResetGame();
            break;

		case PLAY_GAME:
			PlayGame();
			break;

        case PAUSE:
            PauseGame();
            break;

		case NEXT_LEVEL:
			NextLevel();
			break;

        case GAME_OVER:
            GameOver();
            break;

		default:
			break;

	}

}/* ==== End of: void cdx_DoFrame( void )  ================================ */


// ---------------------------------------------------------------------------
//
// Name:        DisplaySplash      
//
// Function:    Displays Logo and Plays Thunder Sound. This is a 
//              GameState function.     GameState = SPLASH
//                  
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void DisplaySplash( void )
{
    if ( !g_bFullScreen )
	    GameState = INTRO;
    else
        GameState = INTRO; //should be SPLASH ????????????

}/* ==== End of: void DisplaySplash( void )  ============================== */



// ---------------------------------------------------------------------------
//
// Name:        GameIntro    
//
// Function:    Displays Intro screen with points and Title. This is a 
//              GameState function.     GameState = INTRO
//              
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void GameIntro( void )
{
	HDC  dc;   
	UINT DefaultMode;

	Back->Fill(0); // Clear background

	/***
    Put some Stars
    ***/
	if( bRestorePalette ) // Only the first time afer the Logo scene !
	{
		bRestorePalette = false;
		Screen->Flip();
//		Screen->SetPalette(0, 256, pe); // Restore our palette. 
	}
    if( PCStars->GetValue() >= STAR_DELAY )
    {
	    Stars->Update(); // Update stars
        PCStars->Reset();
    }
    else
    {
        Stars->DrawStars();
    }

	/***
    Display Text
    ***/
	dc = Back->GetDC(); 
	DefaultMode = GetTextAlign(dc);
    SetTextAlign(dc, TA_CENTER | VTA_CENTER);
//	Back->SetFont(); //??????????????????????????????????? what font
	Back->TextXY(ScreenWidth/2, ScreenHeight-20, WHITE, MirzaKingText); // TextXY(x, y, color, string)

	int startatY = ScreenHeight/2-100;
	for( int i=0; i<4; i++ )
	{
		Back->TextXY(ScreenWidth/2, startatY, WHITE, PointsText[i]);
		startatY += 50;
	}

	/***
    Draw menu
    ***/
	CurrentMenu->Draw(ScreenWidth/2, ScreenHeight-140); //X,Y
	SetTextAlign(dc, DefaultMode);
	Back->ReleaseDC();

	/***
    Draw Title
    ***/
	Title->SetPosX(ScreenWidth/2-175);
	Title->SetPosY(0);
	Title->Draw(Back , 0 , 0 , CDXBLT_TRANS );

	/***
    Draw Invaders & ufo
    ***/
	SI->SetFrame( 8 );	// Green Invader
	SI->SetPos(ScreenWidth/2-85,ScreenHeight/2+45);	
	SI->Draw(Back , 0 , 0 , CDXBLT_TRANS );
	SI->SetFrame( 4 );
	SI->SetPos(ScreenWidth/2-85,ScreenHeight/2-5);
	SI->Draw(Back , 0 , 0 , CDXBLT_TRANS );
	SI->SetFrame( 9 ); //Red Invader
	SI->SetPos(ScreenWidth/2-85,ScreenHeight/2-55);
	SI->Draw(Back , 0 , 0 , CDXBLT_TRANS );
	SI->SetFrame( 10 );//UFO
	SI->SetPos(ScreenWidth/2-85,ScreenHeight/2-105);
	SI->Draw(Back , 0 , 0 , CDXBLT_TRANS );

	Screen->Flip();	

}/* ==== End of: void GameIntro( void )  ================================== */



// ---------------------------------------------------------------------------
//
// Name:        DisplaySetup    
//
// Function:    Video mode setup menu. This is a GameState function.
//              GameState = SETUP
//                  
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void DisplaySetup( void )
{
	Back->Fill(0);

	CurrentMenu->DrawCentered();

	Screen->Flip();

}/* ==== End of: void DisplaySetup( void )  =============================== */


// ---------------------------------------------------------------------------
//
// Name:		ResetShip   
//
// Function:    Rest ship's state and position.
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void ResetShip(void)
{
    Ship->SetPosY( SHIP_Y ); //Position the Ship on Y-axis (constant)
	Ship->SetPosX( 0 );
	Ship->SetFrame(1);
	Ship->SetColorKey(0);
    Ship->SetState( SHIP_ACTIVE );
    Ship->SetDelay(0);

}/* ==== End of: void ResetShip( void )  =============================== */


// ---------------------------------------------------------------------------
//
// Name:        ResetGame    
//
// Function:    Sets all the variables and objects in their starting position.
//              This is a GameState function.     GameState = RESET
//                  
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void ResetGame( void )
{
    /***
    Reset all game variables
    ***/
    if(GameLevel > 20) 
		GameLevel=0;
	BulletsToFire = 1;
	SIMoveDelay = 1000 - GameLevel*25;
    NumberOfLiveSI = HEAD_INVADERS + ANTENA_INVADERS + CONE_INVADERS;
    BulletsSinceLastUFO = NoActiveBombs = 0;
    ShipdX = UFOdX = BombdY = 0.0;
    SIPlayStep = 0;
    SINewFrame = 1; 
    MoveDown = false;

    /***
    Set colors for collision detection !
    Well, this didn't work when dispay was set for 32-bit colors
    so we'll have to change startegy! We'll draw sprites and call GetPixel(x,y)
    to determine color hex number.
    CDX_LPDIRECTDRAWSURFACE surface = Screen->GetBack()->GetDDS();
    WhitePixel  = DDColorMatch( surface, WHITE  ); 
    RedPixel    = DDColorMatch( surface, RED    );
    GreenPixel  = DDColorMatch( surface, GREEN  );
    YellowPixel = DDColorMatch( surface, YELLOW );
    ***/
    // Ufo
    UFOSound->Stop();
    UFO->SetFrame( 1 );
    UFO->SetPos( 0, 0 );
    UFO->Draw(Back, 0,0, CDXBLT_TRANS );
    Back->Lock();
	RedPixel=Back->GetPixel(10,10);//xy
	Back->UnLock();
    // Invader
    SI->SetFrame( 1 );
    SI->SetPos(0,0);
    SI->Draw(Back, 0,0, CDXBLT_TRANS );
    Back->Lock();
	WhitePixel=Back->GetPixel(10,10);//xy
	Back->UnLock();
    // Shield
    SI->SetFrame( 11 );//shield block
	SI->SetPos(0,0);
	SI->Draw(Back , 0 , 0 , CDXBLT_TRANS );
    Back->Lock();
	ShieldPixel=Back->GetPixel(10,10);//xy
	Back->UnLock();
    // Ship
    Ship->SetFrame(1);
    Ship->SetPos(0,0);
    Ship->Draw(Back , 0 , 0 , CDXBLT_TRANS );
    Back->Lock();
	ShipPixel=Back->GetPixel(15,15);//xy
	Back->UnLock();

    /***
    Reset Ship
    ***/
	ResetShip();
    
    /***
    Reset UFO
    ***/
    UFO->SetState( UFO_NOT_ACTIVE );
    UFO->SetFrame( 1 );
    UFO->SetPos( UFO_X, UFO_Y );

    /***
    Reset Bullets
    ***/
    for( int i=0; i < MAX_BULLETS; i++) 
    {
		Bullets[i]->SetFrame(1);
		Bullets[i]->SetState( BULLET_NOT_ACTIVE );
		Bullets[i]->SetColorKey(0); 
	}

    /***
    Reset Bombs
    ***/
    for( i=0; i < MAX_BOMBS; i++) 
    {
		Bombs[i]->SetState( BOMB_NOT_ACTIVE );
		Bombs[i]->SetColorKey(0); 
	}

	/***
    Reset Invaders 
	***/
    SIStartMoveOnX  = 5;  
    SIStopMoveOnX   = ScreenWidth-37;   // limits when moving when to stop and change direction
    SIStopMoveOnY   = SHIP_Y;

    SIStartDrawOnX = 75;  // for initial drawing and line up
    SIStartDrawOnY = 205;
    SIDrawStep     = 30;

    int posX = 0;
	for( int j=0; j < (HEAD_INVADERS+ANTENA_INVADERS+CONE_INVADERS); j++, posX++)
	{
		if( posX == CONE_INVADERS ) posX = 0; //CONE_INVADERS is max number per row

		SInvaders[j]->SetState( SI_ACTIVE );
		SInvaders[j]->SetVelX( SIVelocityRight );
		SInvaders[j]->SetVelY( SIVelocityDown );

		if( j < HEAD_INVADERS/2 )
		{
			SInvaders[j]->SetPos( SIStartDrawOnX + SIDrawStep*posX, SIStartDrawOnY   ); //x,y Head Invaders 1st row
			SInvaders[j]->SetFrame( 1 );
			SInvaders[j]->SetType( SI_HEAD );
		}
		else
		if( j >= HEAD_INVADERS/2 && j < HEAD_INVADERS )
		{
			SInvaders[j]->SetPos( SIStartDrawOnX + SIDrawStep*posX, SIStartDrawOnY-35 ); //x,y Head Invaders 2nd row (from the bottom )
			SInvaders[j]->SetFrame( 1 );
			SInvaders[j]->SetType( SI_HEAD );
		}
		else
		if( j >= HEAD_INVADERS && j < HEAD_INVADERS+ANTENA_INVADERS/2)
		{
			SInvaders[j]->SetPos(SIStartDrawOnX + SIDrawStep*posX, SIStartDrawOnY - 70 ); //x,y Antenas, 3rd row
			SInvaders[j]->SetFrame( 3 );
			SInvaders[j]->SetType( SI_ANTENA );
		}
		else
		if( j >= HEAD_INVADERS+ANTENA_INVADERS/2 && j < HEAD_INVADERS+ANTENA_INVADERS )
		{
			SInvaders[j]->SetPos(SIStartDrawOnX + SIDrawStep*posX, SIStartDrawOnY - 105 ); //x,y Antenas, 4th row
			SInvaders[j]->SetFrame( 3 );
			SInvaders[j]->SetType( SI_ANTENA );
		}
		else
		if( j >= HEAD_INVADERS+ANTENA_INVADERS )
		{
			SInvaders[j]->SetPos(SIStartDrawOnX + SIDrawStep*posX, SIStartDrawOnY - 140 ); //x,y Cones, 5th row
			SInvaders[j]->SetFrame( 5 );
			SInvaders[j]->SetType( SI_CONE );
		}

		SInvaders[j]->SetColorKey( 0 );
	}

    /***
    Reset Shields
    ***/
    for(i=0; i<SHIELD_HEIGHT; i++)
    {
        for(j=0; j<SHIELD_WIDTH; j++)
        {
            Shield1[i][j]=Shield2[i][j]=Shield3[i][j]=Shield4[i][j]=ShieldBitMap[i][j];
        }
    }

    /***
    Reset Timers
    ***/
	PCFrameDuration->Reset();
	PCNextLevel->Reset();

    GameState = NEXT_LEVEL;

}/* ==== End of: void ResetGame( void )  ================================== */


// ---------------------------------------------------------------------------
//
// Name:        PlayGame    
//
// Function:    Function that moves the game. It will first check for the inputs,
//              then move objects based on time it took to render previous frame.
//              After "collision detection", background buffer will be cleared
//              and updated game will be drawn.    
//              This is a GameState function.     GameState = PLAY_GAME
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void PlayGame( void )
{
	CheckInput();         // Check to see if ship is moving or bullets are fired ? 

    MoveObjects();        // Move still active objects to the new positions !

    Back->Fill(0);        // Clear background before drawing new scene !

	DrawObjects();        // Draw new scene !

    DisplayStatistics( ); // Display score, hi-score and number of lives !
    
    if( DebugFlag )
	    Screen->Flip(TRUE,TRUE,TRUE);
    else
        Screen->Flip();

    CollisionDetection(); // Detect collisions in the previous frame and update game !

}/* ==== End of: void PlayGame( void )  =================================== */



// ---------------------------------------------------------------------------
//
// Name:        CheckInput   
//
// Function:    Check if keyboard pressed: update ship position, fire bullet
//                                     or quit.
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void CheckInput( void )
{
    /***
    Get the time it took to render last frame
    ***/
	DurationOfTheFrame = PCFrameDuration->GetValue();
	PCFrameDuration->Reset();

    /***
    Update keyboard input
    ***/
	Input->Update();

    /***
    Quit ?
    ***/
	if (Input->GetKeyState(CDXKEY_ESCAPE)==CDXKEY_PRESS)  // Quit playing
	{
        UFOSound->Stop();
        // Save HiScore 
        if( Score > HiScore )
        {
		    WriteHighScore( Score );
        }
		GameState = INTRO;
        GameLevel = 0;
        Score = 0;
        Lives = 3;
        
		// return out as the code below is invalid now
		return;
	}
    
    /***
    Move Ship if arrow key pressed and ship is still active and we are in the game
    ***/
    if( (Ship->GetState() == SHIP_ACTIVE) && (GameState == PLAY_GAME) )
    {
         double MoveShipNoPixelsThisFrame = ShipVelocityX * (double)DurationOfTheFrame;
         // error correction
         ShipdX += MoveShipNoPixelsThisFrame - (int)MoveShipNoPixelsThisFrame;
         if( ShipdX >= 1.0 )
         {
             MoveShipNoPixelsThisFrame++;
             ShipdX -= 1.0;
         }
        
         if(Input->GetKeyState(CDXKEY_RIGHTARROW) )    // go right
         {
            Ship->SetPosX( Ship->GetPosX()  +  (int)MoveShipNoPixelsThisFrame);  
        		if(Ship->GetPosX() > (ScreenWidth-40) ) 
        			Ship->SetPosX(ScreenWidth-40);  
         }
         if(Input->GetKeyState(CDXKEY_LEFTARROW) )      // go left
         {
            Ship->SetPosX( Ship->GetPosX()  - (int)MoveShipNoPixelsThisFrame ); 
        	   if(Ship->GetPosX() < 0) 
        		   Ship->SetPosX(0); 
         }
    }// if( Ship->GetState() == SHIP_ACTIVE )

    /***
    Fire bullets is 'space' key pressed and ship is still active
    ***/
	int BulletsFired = 0;
	if (Input->GetKeyState(CDXKEY_SPACE) && ( Ship->GetState() == SHIP_ACTIVE ) && (GameState == PLAY_GAME))  // Fire the bulet 
	{ 
		if( PCFireBullet->GetValue() >= BULLET_DELAY )
		{
			for( int i=0; (i < MAX_BULLETS)  ; i++) 
			{
					if( Bullets[i]->GetState() == BULLET_NOT_ACTIVE ) //find a bullet that is currently inactive
					{ 
						Bullets[i]->SetState( BULLET_ACTIVE );
                        Bullets[i]->SetFrame(1);
                        BulletsFired++;
                        BulletsSinceLastUFO++;

						if( BulletsFired == 1 )
						{
							Bullets[i]->SetPosX(Ship->GetPosX() + 8 );
							Bullets[i]->SetPosY(Ship->GetPosY() - 8 );
                            Bullets[i]->SetFrame(1);
						}
						else
						if( BulletsFired == 2 )
						{
							Bullets[i]->SetPosX(Ship->GetPosX() - 6 );
							Bullets[i]->SetPosY(Ship->GetPosY() + 6 );
                            Bullets[i]->SetFrame(1);
						}
						else
						if( BulletsFired == 3 )
						{
							Bullets[i]->SetPosX(Ship->GetPosX() + 22 );
							Bullets[i]->SetPosY(Ship->GetPosY() + 6 );
                            Bullets[i]->SetFrame(1);
						}

						if( SoundFlag && (BulletsFired == 1))
                        {
                            BulletSound->Stop();
							BulletSound->Play();
                        }

                        if( BulletsFired == BulletsToFire ) break;
					}
			}//for( int i=0; (i < MAX_BULLETS) ....

			PCFireBullet->Reset();
            // Move UFO if time to...
            if( (BulletsToFire == 1 && BulletsSinceLastUFO >= ACTIVATE_UFO_ON) ||
                (BulletsToFire == 3 && BulletsSinceLastUFO >= 2*ACTIVATE_UFO_ON))
            {
                BulletsSinceLastUFO = 0;
                UFO->SetPos( UFO_X, UFO_Y );
                UFO->SetState( UFO_ACTIVE );
                if( SoundFlag )
                {   UFOSound->Stop();
                    UFOSound->Play(); 
                }
            }

		}//if( PCFireBullet->GetValue() >= BULLET_DELAY )
	}//if (Input->GetKeyState(CDXKEY_SPACE) )  // Fire the bulet 

    /***
    Flash Keyboard buffer
    ***/
    Input->FlushKeyboardData();

}/* ==== End of: void CheckInput( void )  ================================= */



// ---------------------------------------------------------------------------
//
// Name:        MoveObjects        
//
// Function:    Move all the sprites (except Ship).
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void MoveObjects( void )
{
	int i,j;
    bool bFireBombs = false;

	/***
    Move Bullets
    ***/
	for( i=0; i < MAX_BULLETS; i++) 
	{
		if( Bullets[i]->GetState() == BULLET_ACTIVE ) //find active bullet
		{	
				Bullets[i]->SetPosY( Bullets[i]->GetPosY() - (int)(BulletVelocityY * (double)DurationOfTheFrame) );
				if( Bullets[i]->GetPosY() <= 0  )
					Bullets[i]->SetState( BULLET_NOT_ACTIVE );
		}
	}
	

    /***
    Move Invaders
    ***/
    if( SIMoveDelay > 1000 ) SIMoveDelay = 1000;
    else
    if( SIMoveDelay <   15 ) SIMoveDelay = 15;

	// Check if its time to move invaders
	if( (PCSIMove->GetValue()>=(DWORD)SIMoveDelay)&&(NumberOfLiveSI>0)&&(Ship->GetState()==SHIP_ACTIVE))
	{
        bFireBombs = true;
		if( !MoveDown )
		{
			for( j=0; j < (HEAD_INVADERS+ANTENA_INVADERS+CONE_INVADERS); j++ ) // Then move invaders
			{
				if( SInvaders[j]->GetState() == SI_ACTIVE )
					SInvaders[j]->SetFrame( SInvaders[j]->GetFrame() + SINewFrame );

				SInvaders[j]->SetPosX( SInvaders[j]->GetPosX() + SInvaders[j]->GetVelX() ); //set X position
	
				if( SInvaders[j]->GetState() == SI_ACTIVE && SInvaders[j]->GetPosX() > SIStopMoveOnX )
					MoveDown = true;
				else
				if( SInvaders[j]->GetState() == SI_ACTIVE && SInvaders[j]->GetPosX() < SIStartMoveOnX )
					MoveDown = true;				
			}
			if( MoveDown ) //check if we really want to go down one row
			{
				for(j=0; j<(HEAD_INVADERS+ANTENA_INVADERS+CONE_INVADERS); j++ )
					if( SInvaders[j]->GetPosY() >= SIStopMoveOnY  &&  SInvaders[j]->GetState() == SI_ACTIVE )
						MoveDown = false; // don't move down cause one or more is already at the bottom

				if( !MoveDown ) //but instead change the direction
					for(j=0; j<(HEAD_INVADERS+ANTENA_INVADERS+CONE_INVADERS); j++ )
						SInvaders[j]->SetVelX( SInvaders[j]->GetVelX() * (-1) );
			}
		}
		else // move 'em down
		{
			for( int k=0; k < (HEAD_INVADERS+ANTENA_INVADERS+CONE_INVADERS); k++ ) 
			{
				if( MoveDown )
					SInvaders[k]->SetPosY( SInvaders[k]->GetPosY() + SInvaders[k]->GetVelY() );
				SInvaders[k]->SetVelX( SInvaders[k]->GetVelX() * (-1) );
			}
			MoveDown = false;
		}

		PCSIMove->Reset();

		if( !MoveDown )
			SINewFrame = SINewFrame * (-1); //alternate between two frames

		// Play sound
        if( SoundFlag  )
		{
			if( SIPlayStep == STEP1 )
			{
                Step4->Stop();
				Step1->Play();
				SIPlayStep = STEP2;
			}
			else 
			if( SIPlayStep == STEP2 )
			{	
                Step1->Stop();
				Step2->Play();
				SIPlayStep = STEP3;
			}
			else
			if( SIPlayStep == STEP3 )
			{
                Step2->Stop();
				Step3->Play();
				SIPlayStep = STEP4;
			}
			else
			if( SIPlayStep == STEP4 )
			{   
                Step3->Stop();
				Step4->Play();
				SIPlayStep = STEP1;
			}
		}
		
	}

    /***
    Move UFO
    ***/
    if( UFO->GetState() == UFO_ACTIVE  )
    {   
        double MoveUFOPixelsThisFrame =  UFOVelocityX * (double)DurationOfTheFrame; 
        UFOdX += MoveUFOPixelsThisFrame - (int)MoveUFOPixelsThisFrame;
        // A little bit of correction
        if( UFOdX >= 1.0)
        {
            MoveUFOPixelsThisFrame++;
            UFOdX -= 1.0;
        }

        UFO->SetPosX( UFO->GetPosX() + (int)MoveUFOPixelsThisFrame );

        if( UFO->GetPosX() > ScreenWidth )
        {
            UFO->SetState( UFO_NOT_ACTIVE );
            UFO->SetPos( UFO_X, UFO_Y );
            UFOSound->Stop();
        }
    }

    /***
    Move Bombs & Fire new ones, a little bit of AI
    ***/
    double MoveBombPixelsThisFrame =  BombVelocityY * (double)DurationOfTheFrame; 
    BombdY += MoveBombPixelsThisFrame - (int)MoveBombPixelsThisFrame;
    // A little bit of correction
    if( BombdY >= 1.0)
    {
        MoveBombPixelsThisFrame++;
        BombdY -= 1.0;
    }
    for( i=0; i<MAX_BOMBS; i++ )  // first move active ones
    {
        if( Bombs[i]->GetState() == BOMB_ACTIVE )
        {
            Bombs[i]->SetPosY( Bombs[i]->GetPosY() + (int)MoveBombPixelsThisFrame);
            if( Bombs[i]->GetPosY() >= ScreenHeight )
            {
                Bombs[i]->SetState( BOMB_NOT_ACTIVE );
                NoActiveBombs--;
            }
        }
    }
    /***
    Now, Fire Bombs
    ***/
    if( bFireBombs && (NoActiveBombs < (3+GameLevel))) // now, fire new ones if time for it.
    {
        if( NumberOfLiveSI <= 2 )
        {
            for(i=0; i<HEAD_INVADERS + ANTENA_INVADERS + CONE_INVADERS; i++)
            {
                if( SInvaders[i]->GetState() == SI_ACTIVE )
                {
                    for(j=0; j<MAX_BOMBS; j++)
                    {
                        if( Bombs[j]->GetState() == BOMB_NOT_ACTIVE )
                        {
                            Bombs[j]->SetState( BOMB_ACTIVE );
                            NoActiveBombs++;
                            Bombs[j]->SetFrame(3);
                            Bombs[j]->SetPos( SInvaders[i]->GetPosX()+7, SInvaders[i]->GetPosY()+25 );
                            break; // go find next invader to shot
                        }
                    }
                }
            }
        }
        else
        {  
            int BombsToFire;
            int fired = 0;
            if( NumberOfLiveSI >= (HEAD_INVADERS + ANTENA_INVADERS + CONE_INVADERS)/2 ) BombsToFire=3+GameLevel;
            else BombsToFire = 2+GameLevel;
            if( BombsToFire > NumberOfLiveSI )  BombsToFire = NumberOfLiveSI;
            // Find if any invader is positioned above the ship
            int ShipX = Ship->GetPosX();
            for(i=0; (i<HEAD_INVADERS + ANTENA_INVADERS + CONE_INVADERS) && (fired<(BombsToFire-2)); i++)
            {
                int SIPositionX = SInvaders[i]->GetPosX();
                if( (SIPositionX < (ShipX+32+SIDrawStep) && SIPositionX > (ShipX-(32+SIDrawStep))) && SInvaders[i]->GetState() == SI_ACTIVE  )
                {
                    for(j=0; j<MAX_BOMBS; j++)
                    {
                        if( Bombs[j]->GetState() == BOMB_NOT_ACTIVE )
                        {
                            Bombs[j]->SetState( BOMB_ACTIVE );
                            NoActiveBombs++;
                            Bombs[j]->SetFrame(3);
                            Bombs[j]->SetPos( SInvaders[i]->GetPosX()+7, SInvaders[i]->GetPosY()+25 );
                            fired++;
                            break; // go find next invader to shot
                        }
                    }
                }
            }

            while( fired < BombsToFire ) // If we still have to fire some bombs, than do it random 
            {
                int num = (HEAD_INVADERS + ANTENA_INVADERS + CONE_INVADERS -1) * rand() / (RAND_MAX + 1); // get random number in the range

                if( num >= 0 && num < HEAD_INVADERS + ANTENA_INVADERS + CONE_INVADERS ) //just to make sure we are in the range of SInvaders array
                {
                    if( SInvaders[num]->GetState() == SI_ACTIVE )
                    {
                        for(j=0; j<MAX_BOMBS; j++)
                        {
                            if( Bombs[j]->GetState() == BOMB_NOT_ACTIVE )
                            {
                                Bombs[j]->SetState( BOMB_ACTIVE );
                                NoActiveBombs++;
                                Bombs[j]->SetFrame(3);
                                Bombs[j]->SetPos( SInvaders[num]->GetPosX()+7, SInvaders[num]->GetPosY()+25 );
                                fired++;
                                break; // go find next invader to shot
                            }
                        }
                    }
                }
            }
        }
    }// end if( bFireBombs )

    /***
    Move Torpedos
    ***/


}/* ==== End of: void MoveObjects( void )  ================================ */


// ---------------------------------------------------------------------------
//
// Name:         DrawObjects
//
// Function:     Draw all of the game objects to the back buffer in the following
//               order: bullets, bombs, torpedos, ship, shield, invaders, ufo.
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void DrawObjects( void )
{	
    int i,j;	
	/***
    Draw Bullets First
    ***/
	for( i=0; i < MAX_BULLETS; i++) 
	{
        if( Bullets[i]->GetState() != BULLET_NOT_ACTIVE ) //find an unused spot in the array
		{ 
            Bullets[i]->Draw(Back, 0,0, CDXBLT_TRANS );

            if(  (Bullets[i]->GetState() == BULLET_EXPLODE )&&(PCBulletExplode->GetValue()-Bullets[i]->GetDelay() >= BULLET_EXPLODE_DELAY) )
                Bullets[i]->SetState( BULLET_NOT_ACTIVE );
			
		}
	}

    /***
    Draw Bombs
    ***/
    for( i=0; i<MAX_BOMBS; i++ )
        if( Bombs[i]->GetState() != BOMB_NOT_ACTIVE )
        {
            Bombs[i]->Draw(Back,0,0,CDXBLT_TRANS);

            if(  (Bombs[i]->GetState() == BOMB_EXPLODE )&&(PCBulletExplode->GetValue()-Bombs[i]->GetDelay() >= BOMB_EXPLODE_DELAY) )
            {
                Bombs[i]->SetState( BOMB_NOT_ACTIVE );
                NoActiveBombs--;
            }
        }

    /***
    Draw Torpedos
    ***/

    /***
    Draw Ship
    ***/
    if( (Ship->GetState()==SHIP_EXPLODE)&&(PCShipExplode->GetValue()-Ship->GetDelay() >= SHIP_EXPLODE_DELAY) )
    {

        if( Ship->GetFrame() < SHIP_LAST_FRAME )
        {
            Ship->SetFrame( Ship->GetFrame() + 1 );
            Ship->SetDelay( PCShipExplode->GetValue() );
        }
        else
        {
            UFOSound->Stop();
            Ship->SetFrame( 0 );
            Ship->SetState(SHIP_NOT_ACTIVE);

            if( Lives > 0 )
            {
				ResetShip();
			}
            else
            {   
                GameState = GAME_OVER;
                CreditsScroll->ResetPosition();
            }
        }
    }
	Ship->Draw(Back, 0,0, CDXBLT_TRANS ); 

    /***
    Draw UFO
    ***/
    if( UFO->GetState() != UFO_NOT_ACTIVE )
    {   
        if( UFO->GetState() == UFO_ACTIVE )
        {
            UFO->Draw(Back, 0,0, CDXBLT_TRANS );
        }
        else  // Display points
        {
            Back->GetDC();
            Back->TextXY( UFO->GetPosX(), UFO->GetPosY(), RED, strUFOScore); 
            Back->ReleaseDC();
            if( PCUFOExplosion->GetValue() >= UFO_EXPLODE_DELAY )
            {
                UFO->SetState( UFO_NOT_ACTIVE );
                ExplodeUFO->Stop();
            }
        }
    }


    /***
    Draw Shields
    ***/
    int a,b,c,d;
    Back->Lock();
    for(i=0; i<SHIELD_HEIGHT; i++)
    {
        for(a=0; a<SHIELD_WIDTH; a++)
            if( Shield1[i][a] == 1 )
                Back->PutPixel( SHIELD_1_X+a, SHIELD_Y+i, ShieldPixel);

        for(b=0; b<SHIELD_WIDTH; b++)
            if( Shield2[i][b] == 1 )
                Back->PutPixel( SHIELD_2_X+b, SHIELD_Y+i, ShieldPixel);

        for(c=0; c<SHIELD_WIDTH; c++)
            if( Shield3[i][c] == 1 )
                Back->PutPixel( SHIELD_3_X+c, SHIELD_Y+i, ShieldPixel);

        for(d=0; d<SHIELD_WIDTH; d++)
            if( Shield4[i][d] == 1 )
                Back->PutPixel( SHIELD_4_X+d, SHIELD_Y+i, ShieldPixel);

    }
    Back->UnLock();

    /***
    Draw invaders
    ***/
	for( j=0; j < (HEAD_INVADERS+ANTENA_INVADERS+CONE_INVADERS); j++ ) 
	{
		if( SInvaders[j]->GetState() != SI_NOT_ACTIVE )
		{
			SInvaders[j]->Draw(Back, 0,0, CDXBLT_TRANS );

			if ( SInvaders[j]->GetState() == SI_EXPLODE ) //If just hit
			{
				SInvaders[j]->SetState( SI_EXPLODE_DELAY ); //show explosion frame number of millisec
				SInvaders[j]->SetDelay(PCSIExplosion->GetValue());
				if( SoundFlag )
				{
                    ExplodeSI->Stop();
					ExplodeSI->Play();
				}
			}
			else
			if( (PCSIExplosion->GetValue()-SInvaders[j]->GetDelay() >= EXPLODE_DELAY)&&(SInvaders[j]->GetState() == SI_EXPLODE_DELAY) )
			{	
				SInvaders[j]->SetState( SI_NOT_ACTIVE );
                SIMoveDelay-=10;
                if( NumberOfLiveSI <= 0 )
                {
                    UFOSound->Stop();
                    GameLevel++;
                    GameState = RESET;// No more invaders to kill, so bring 'em back
                    PCSIExplosion->WaitFor(TIME_TO_WAIT_INVADERS);

                }
            }
		}

	}	


}/* ==== End of: void DrawObjects( void )  ================================ */


// ---------------------------------------------------------------------------
//
// Name:      CollisionDetection       
//
// Function:  This function handles all collision detection between the sprites.
//            Possible collisions:  Bullet -> Shield, Torpedo, Bomb, Invader, UFO
//                                  Bomb,Torpedo -> Bullet, Shield, Ship
//            The goal is to minimize number of comparisons, so if we have already 
//            search for Bullet-Bomb collision we won't do it again as Bomb-Bullet
//            detection.
//      
//            Search patterns:
//                          1 -  Bullet->Shield->Invader,Bomb,Torpedo->UFO
//                          2 -  Bomb->Shield->Ship
//                          3 -  Torpedo->Shield->Ship
//                          4 -  Invader->Shield->Ship
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void CollisionDetection( void )
{
    int i,j;
	bool BulletHit = true;
    bool BulletHasRedPixel = false;
    bool BulletHasWhitePixel = false;
    bool BulletHasShieldPixel = false;
    bool BombHasShieldPixel = false;
    bool BombHasShipPixel = false;
    RECT ShipRect = { Ship->GetPosX()+1, Ship->GetPosY()+1,  Ship->GetPosX()+30,  Ship->GetPosY()+30 };

    /***
    First test all the bullets for collision with: Shield, Invader, Bomb, Torpedo or UFO
    ***/
	for(  int BulletCounter = 0; BulletCounter < MAX_BULLETS; BulletCounter++)
    {
		if( Bullets[BulletCounter]->GetState() == BULLET_ACTIVE )
		{
            // Get bullet rectangle
            RECT BulletRect = { Bullets[BulletCounter]->GetPosX()+7, Bullets[BulletCounter]->GetPosY(),
                 Bullets[BulletCounter]->GetPosX() + 8, // make it narrow
                 Bullets[BulletCounter]->GetPosY() + 15 };

            // Get bullet Pixel colors and set color flags
            Back->Lock();
            for( i=0; i<2; i++ )
                for( j=0; j<16; j++ )
                {
                    if( Bullets[BulletCounter]->GetPosY() > 35 )
                    {
                        DWORD Temp = Back->GetPixel( Bullets[BulletCounter]->GetPosX() + i+7,  Bullets[BulletCounter]->GetPosY()+ j);
                        if( Temp == ShieldPixel ) BulletHasShieldPixel = true;
                        else
                        if( Temp == WhitePixel ) BulletHasWhitePixel = true;
                        else
                        if( Temp == RedPixel ) BulletHasRedPixel = true;
                    }
                }
            Back->UnLock();

            // First Check if the shield has been hit
			if( BulletHasShieldPixel )
			{
                int x,y,a,b,k;
                int Xdir = 1;
                int PixelsOff = 250;
                int Radius = 15;

                for(k=0; k<PixelsOff; k++)
                {
                    // turn PixelsOff pixels off in radius of Radius from tip of the bullet rect 
                    a = Radius*rand() / (RAND_MAX+1);
                    b = Radius*rand() / (RAND_MAX+1);

                    x = a * Xdir + BulletRect.left; // X coord of the pixel we'll turn off
                    y = BulletRect.top+8 - b;       // Y coord of the pixel we'll turn off


                    if( sqrt(a*a + b*b) <= Radius ) // Make the explosion look real.  Turn pixels off in a shape of circl. Pitagora car !
                    {
                        // Find which shield bullet has hit ? First ?
                        if ( BulletRect.left >= (SHIELD_1_X-1)  &&  BulletRect.right <= (SHIELD_1_X+SHIELD_WIDTH) )
                        {
                                // FIND THAT PIXEL IN THE BITMAP AND TURN IT OFF
                                if( ((x-SHIELD_1_X) >=  0) && ((x-SHIELD_1_X) < SHIELD_WIDTH) && ((y-SHIELD_Y) >=  0) && ((y-SHIELD_Y) < SHIELD_HEIGHT ) )
                                    Shield1[ y-SHIELD_Y ][ x-SHIELD_1_X ] = 0;
                        }
                        
                        // Second ?
                        if ( BulletRect.left >= (SHIELD_2_X-1)  &&  BulletRect.right <= (SHIELD_2_X+SHIELD_WIDTH) )
                        {
                                if( ((x-SHIELD_2_X) >=  0) && ((x-SHIELD_2_X) < SHIELD_WIDTH) && ((y-SHIELD_Y) >=  0) && ((y-SHIELD_Y) < SHIELD_HEIGHT ) )
                                    Shield2[ y-SHIELD_Y ][ x-SHIELD_2_X ] = 0;
                        }
                        
                        // Third ?
                        if ( BulletRect.left >= (SHIELD_3_X-1)  &&  BulletRect.right <= (SHIELD_3_X+SHIELD_WIDTH) )
                        {
                                if( ((x-SHIELD_3_X) >=  0) && ((x-SHIELD_3_X) < SHIELD_WIDTH) && ((y-SHIELD_Y) >=  0) && ((y-SHIELD_Y) < SHIELD_HEIGHT ) )
                                    Shield3[ y-SHIELD_Y ][ x-SHIELD_3_X ] = 0;
                        }
                        
                        // Fourth ?
                        if ( BulletRect.left >= (SHIELD_4_X-1)  &&  BulletRect.right <= (SHIELD_4_X+SHIELD_WIDTH) )
                        {
                                if( ((x-SHIELD_4_X) >=  0) && ((x-SHIELD_4_X) < SHIELD_WIDTH) && ((y-SHIELD_Y) >=  0) && ((y-SHIELD_Y) < SHIELD_HEIGHT ) )
                                    Shield4[ y-SHIELD_Y ][ x-SHIELD_4_X ] = 0;
                        }
                        
                        Xdir*=(-1); // Center of explosion is close to the center of the bullet so we want to turn pixels off
                    }                   // to the left and right equaly.
                }// end of for(k=0; k<PixelsOff; k++)

                Bullets[BulletCounter]->SetState( BULLET_EXPLODE );	
                Bullets[BulletCounter]->SetFrame(2);
                Bullets[BulletCounter]->SetPosY( Bullets[BulletCounter]->GetPosY()-6 );
                Bullets[BulletCounter]->SetDelay( PCBulletExplode->GetValue() );

            }// end if( BulletHasShieldPixel )
				
             // If bullet is still active, Invader,Bullet,Torpedo is white so check if any of bullet pixel is turned to be white too.
			if( BulletHasWhitePixel && Bullets[BulletCounter]->GetState() == BULLET_ACTIVE )
			{
                // Now find out which Invader got hit?
                for(  j=0; j < HEAD_INVADERS+ANTENA_INVADERS+CONE_INVADERS; j++ )
				{
					if( SInvaders[j]->GetState() == SI_ACTIVE ) //if active check if that is the one that got hit
					{   // MAKE SI Rectangle
						RECT SIRect = { SInvaders[j]->GetPosX() +  4, SInvaders[j]->GetPosY()+7,
										SInvaders[j]->GetPosX() + 27, // make it smaller
										SInvaders[j]->GetPosY() + 22 };
                        // Perform rectangle collision test
						if(SIRect.top > BulletRect.bottom)      BulletHit = false;
						else if(SIRect.bottom < BulletRect.top) BulletHit = false;
						else if(SIRect.right < BulletRect.left) BulletHit = false;
						else if(SIRect.left > BulletRect.right) BulletHit = false;

	                    if( BulletHit ) // If that is the one .....
						{
                            Bullets[BulletCounter]->SetState( BULLET_NOT_ACTIVE );	// we got hit, so return this bullet to the pool
							SInvaders[j]->SetState( SI_EXPLODE );
							SInvaders[j]->SetFrame( 7 );
                            NumberOfLiveSI--;
                            assert( NumberOfLiveSI >=0 );
							int WhichSI = SInvaders[j]->GetType();
							if( WhichSI == SI_HEAD ) Score+=10;
							else
							if( WhichSI == SI_ANTENA ) Score+=20;
							else
							if( WhichSI == SI_CONE ) Score+=30;
							break;     //... stop looking
						}

						BulletHit = true;
					}// if( SInvaders[j]->GetState() == SI_ACTIVE )
				}//for( int j=0; j < HEAD_INVADERS+ANTENA_INVADERS+CONE_INVADERS; j++ )

                if( Bullets[BulletCounter]->GetState() == BULLET_ACTIVE ) //so, this one didn't hit Invader - maybe bomb ?
                {
                    // check all active bombs for rectangle test
                    for(i=0; i<MAX_BOMBS; i++)
                    {
                        if( Bombs[i]->GetState() == BOMB_ACTIVE )
		                {
                            // Get Bomb rectangle
                            RECT BombRect = { Bombs[i]->GetPosX()+7, Bombs[i]->GetPosY()+3,
                            Bombs[i]->GetPosX() + 9, // make it narrow
                            Bombs[i]->GetPosY() + 12 };

                            // Perform rectangle collision test
						    if(BombRect.top > BulletRect.bottom)      BulletHit = false;
						    else if(BombRect.bottom < BulletRect.top) BulletHit = false;
						    else if(BombRect.right < BulletRect.left) BulletHit = false;
						    else if(BombRect.left > BulletRect.right) BulletHit = false;

	                        if( BulletHit ) // If that is the one .....
						    {
							    Bullets[BulletCounter]->SetState( BULLET_EXPLODE );	
                                Bullets[BulletCounter]->SetFrame(2);
                                Bullets[BulletCounter]->SetPosY( Bullets[BulletCounter]->GetPosY()-6 );
                                Bullets[BulletCounter]->SetDelay( PCBulletExplode->GetValue() );
                                Bombs[i]->SetState( BOMB_EXPLODE );
							    Bombs[i]->SetFrame( 2 );
                                Bombs[i]->SetDelay( PCBulletExplode->GetValue() );
    							break;     //... stop looking
	    					}
    
	    					BulletHit = true;
                        }
                    }

                }

                if( Bullets[BulletCounter]->GetState() == BULLET_ACTIVE ) //so, this one didn't hit bomb - maybe torpedo ?
                {
                    // check all active bombs for rectangle test
                }

			}//if( (Pixel1 ==  WhitePixel ||....... 

            // If at this point bullet is still active, check the UFO
            if( Bullets[BulletCounter]->GetState() == BULLET_ACTIVE && UFO->GetState() == UFO_ACTIVE  && BulletHasRedPixel)
            {
                    Bullets[BulletCounter]->SetState( BULLET_NOT_ACTIVE );// UFO is down !!!
                    UFO->SetState( UFO_EXPLODE );
                    PCUFOExplosion->Reset();
                    UFOSound->Stop();
                    if( SoundFlag )
                        ExplodeUFO->Play();
                    if( BulletsSinceLastUFO == 0 )
                    {
                        Score+=500;
                        sprintf(strUFOScore,   "%d",  500);
                    }
                    else
                    if( BulletsSinceLastUFO == 1 )
                    {
                        Score+=300;
                        sprintf(strUFOScore,   "%d",  300);
                    }
                    else
                    if( BulletsSinceLastUFO == 2 )
                    {
                        Score+=150;
                        sprintf(strUFOScore,   "%d",  150);
                    }
                    else
                    if( BulletsSinceLastUFO >= 2 )
                    {
                        Score+=50;
                        sprintf(strUFOScore,   "%d",   50);
                    }
            }

		}//if( Bullets[BulletCounter]->GetState() == BULLET_ACTIVE )

        BulletHasWhitePixel=BulletHasShieldPixel=BulletHasRedPixel=false;

    }//for( int i=0; i < MAX_BULLETS; i++)


    /***
    Second test all the bombs for collision with: Shield or Ship (first ship, if true don't need to search shield array)
    ***/
    for(  int BombCounter = 0; BombCounter < MAX_BOMBS; BombCounter++)
    {
		if( Bombs[BombCounter]->GetState() == BOMB_ACTIVE )
		{
            // Get Bomb rectangle
            RECT BombRect = { Bombs[BombCounter]->GetPosX()+7, Bombs[BombCounter]->GetPosY()+3,
                 Bombs[BombCounter]->GetPosX() + 9, // make it narrow
                 Bombs[BombCounter]->GetPosY() + 12 };

            // Get Bomb Pixel colors and set color flags
            Back->Lock();
            for( i=7; i<10; i++ )
                for( j=3; j<13; j++ )
                {
                    if( (Bombs[BombCounter]->GetPosY()+ j) < ScreenHeight ) // !!! Danger - Make sure you are not reading not existing pixels !
                    {
                        DWORD Temp = Back->GetPixel( Bombs[BombCounter]->GetPosX() + i,  Bombs[BombCounter]->GetPosY()+ j);
                        if( Temp == ShieldPixel ) BombHasShieldPixel = true;
                        else
                        if( Temp == ShipPixel ) BombHasShipPixel = true;
                    }
                }
            Back->UnLock();

            // First Check if the shield has been hit
			if( BombHasShieldPixel )
			{
                int x,y,a,b,k;
                int Xdir = 1;
                int PixelsOff = 250;
                int Radius = 15;

                for(k=0; k<PixelsOff; k++)
                {
                    // turn PixelsOff pixels off in radius of Radius from tip of the Bomb rect 
                    a = Radius*rand() / (RAND_MAX+1);
                    b = Radius*rand() / (RAND_MAX+1);

                    x = a * Xdir + BombRect.left; // X coord of the pixel we'll turn off
                    y = BombRect.bottom + b;       // Y coord of the pixel we'll turn off


                    if( sqrt(a*a + b*b) <= Radius ) // Make the explosion look real.  Turn pixels off in a shape of circl. Pitagora car !
                    {
                        // Find which shield Bomb has hit ? First ?
                        if ( BombRect.left >= (SHIELD_1_X-1)  &&  BombRect.right <= (SHIELD_1_X+SHIELD_WIDTH) )
                        {
                                // FIND THAT PIXEL IN THE BITMAP AND TURN IT OFF
                                if( ((x-SHIELD_1_X) >=  0) && ((x-SHIELD_1_X) < SHIELD_WIDTH) && ((y-SHIELD_Y) >=  0) && ((y-SHIELD_Y) < SHIELD_HEIGHT ) )
                                    Shield1[ y-SHIELD_Y ][ x-SHIELD_1_X ] = 0;
                        }
                        
                        // Second ?
                        if ( BombRect.left >= (SHIELD_2_X-1)  &&  BombRect.right <= (SHIELD_2_X+SHIELD_WIDTH) )
                        {
                                if( ((x-SHIELD_2_X) >=  0) && ((x-SHIELD_2_X) < SHIELD_WIDTH) && ((y-SHIELD_Y) >=  0) && ((y-SHIELD_Y) < SHIELD_HEIGHT ) )
                                    Shield2[ y-SHIELD_Y ][ x-SHIELD_2_X ] = 0;
                        }
                        
                        // Third ?
                        if ( BombRect.left >= (SHIELD_3_X-1)  &&  BombRect.right <= (SHIELD_3_X+SHIELD_WIDTH) )
                        {
                                if( ((x-SHIELD_3_X) >=  0) && ((x-SHIELD_3_X) < SHIELD_WIDTH) && ((y-SHIELD_Y) >=  0) && ((y-SHIELD_Y) < SHIELD_HEIGHT ) )
                                    Shield3[ y-SHIELD_Y ][ x-SHIELD_3_X ] = 0;
                        }
                        
                        // Fourth ?
                        if ( BombRect.left >= (SHIELD_4_X-1)  &&  BombRect.right <= (SHIELD_4_X+SHIELD_WIDTH) )
                        {
                                if( ((x-SHIELD_4_X) >=  0) && ((x-SHIELD_4_X) < SHIELD_WIDTH) && ((y-SHIELD_Y) >=  0) && ((y-SHIELD_Y) < SHIELD_HEIGHT ) )
                                    Shield4[ y-SHIELD_Y ][ x-SHIELD_4_X ] = 0;
                        }
                        
                        Xdir*=(-1); // Center of explosion is close to the center of the Bomb so we want to turn pixels off
                    }                   // to the left and right equaly.
                }// end of for(k=0; k<PixelsOff; k++)

                Bombs[BombCounter]->SetState( BOMB_EXPLODE );	
                Bombs[BombCounter]->SetFrame(2);
                Bombs[BombCounter]->SetDelay( PCBulletExplode->GetValue() );

            }// end if( BombHasShieldPixel ) 
            
            if( (Bombs[BombCounter]->GetState()==BOMB_ACTIVE)  &&  BombHasShipPixel )
            {
                if( Ship->GetState()==SHIP_ACTIVE )
                {
                    Ship->SetState( SHIP_EXPLODE );
                    Lives--;
                    if( SoundFlag )
                        ExplodeShip->Play();
                }
                Bombs[BombCounter]->SetState( BOMB_EXPLODE );	
                Bombs[BombCounter]->SetFrame(2);
                Bombs[BombCounter]->SetDelay( PCBulletExplode->GetValue() );
                return;
            }

        }// end if( Bombs[BombCounter]->GetState() == BOMB_ACTIVE )
        BombHasShieldPixel = BombHasShipPixel = false;
    }// end for(  int BombCounter = 0; 
    
    /***
    Third test all the torpedos for collision with: Shield or Ship
    ***/

    /***
    Fourth test all the active Invaders for collision with: Shield, Ship
    ***/
    for(int SICounter=0; SICounter<HEAD_INVADERS+ANTENA_INVADERS+CONE_INVADERS; SICounter++ )
    {
        if( SInvaders[SICounter]->GetState() == SI_ACTIVE ) //if active check if that is the one that got hit
		{   // MAKE SI Rectangle with exact height
			RECT SIRect = { SInvaders[SICounter]->GetPosX() +  4, SInvaders[SICounter]->GetPosY() +  7,
							SInvaders[SICounter]->GetPosX() + 27, SInvaders[SICounter]->GetPosY() + 22 };
                        
            // Perform rectangle collision test with shield
            if( SIRect.bottom > SHIELD_Y && SIRect.top < (SHIELD_Y+SHIELD_HEIGHT-15) )
            {
                //First ?
                        if ( SIRect.left >= (SHIELD_1_X-1)  ||  SIRect.right <= (SHIELD_1_X+SHIELD_WIDTH) )
                        {
                                // FIND THAT PIXEL IN THE BITMAP AND TURN IT OFF
                                for(int y=SIRect.top; y<SIRect.bottom; y++ )  
                                  for(int x=SIRect.left; x<SIRect.right; x++)
                                    if( ((x-SHIELD_1_X) >=  0) && ((x-SHIELD_1_X) < SHIELD_WIDTH) && ((y-SHIELD_Y) >=  0) && ((y-SHIELD_Y) < SHIELD_HEIGHT ) )
                                        Shield1[ y-SHIELD_Y ][ x-SHIELD_1_X ] = 0;
                        }
                        
                        // Second ?
                        if ( SIRect.left >= (SHIELD_2_X-1)  ||  SIRect.right <= (SHIELD_2_X+SHIELD_WIDTH) )
                        {
                            for(int y=SIRect.top; y<SIRect.bottom; y++ )  
                                  for(int x=SIRect.left; x<SIRect.right; x++)
                                    if( ((x-SHIELD_2_X) >=  0) && ((x-SHIELD_2_X) < SHIELD_WIDTH) && ((y-SHIELD_Y) >=  0) && ((y-SHIELD_Y) < SHIELD_HEIGHT ) )
                                        Shield2[ y-SHIELD_Y ][ x-SHIELD_2_X ] = 0;
                        }
                        
                        // Third ?
                        if ( SIRect.left >= (SHIELD_3_X-1)  ||  SIRect.right <= (SHIELD_3_X+SHIELD_WIDTH) )
                        {
                            for(int y=SIRect.top; y<SIRect.bottom; y++ )  
                                  for(int x=SIRect.left; x<SIRect.right; x++)
                                    if( ((x-SHIELD_3_X) >=  0) && ((x-SHIELD_3_X) < SHIELD_WIDTH) && ((y-SHIELD_Y) >=  0) && ((y-SHIELD_Y) < SHIELD_HEIGHT ) )
                                        Shield3[ y-SHIELD_Y ][ x-SHIELD_3_X ] = 0;
                        }
                        
                        // Fourth ?
                        if ( SIRect.left >= (SHIELD_4_X-1)  ||  SIRect.right <= (SHIELD_4_X+SHIELD_WIDTH) )
                        {
                            for(int y=SIRect.top; y<SIRect.bottom; y++ )  
                                  for(int x=SIRect.left; x<SIRect.right; x++)
                                    if( ((x-SHIELD_4_X) >=  0) && ((x-SHIELD_4_X) < SHIELD_WIDTH) && ((y-SHIELD_Y) >=  0) && ((y-SHIELD_Y) < SHIELD_HEIGHT ) )
                                        Shield4[ y-SHIELD_Y ][ x-SHIELD_4_X ] = 0;
                        }
            }

            // Perform rectangle collision test with ship
            if( (SIRect.bottom > SHIP_Y) && (Ship->GetState() == SHIP_ACTIVE) )
            {
                bool RectCollision  = true;

                if( SIRect.bottom > (SHIP_Y+20) )
                {
                    // Perform rectangle collision test with full ship rect
				    if(SIRect.top > ShipRect.bottom)      RectCollision = false;
				    else if(SIRect.bottom < ShipRect.top) RectCollision = false;
				    else if(SIRect.right < ShipRect.left) RectCollision = false;
				    else if(SIRect.left > ShipRect.right) RectCollision = false;
                }
                else
                {
                    // Perform rectangle collision test narrowing ship rect
				    if(SIRect.top > ShipRect.bottom)      RectCollision = false;
				    else if(SIRect.bottom < ShipRect.top) RectCollision = false;
				    else if(SIRect.right < ShipRect.left+10) RectCollision = false;
				    else if(SIRect.left > ShipRect.right-10) RectCollision = false;
                }

                if(RectCollision)
                {
                    Ship->SetState( SHIP_EXPLODE );
                    Lives--;
                    if( SoundFlag )
                        ExplodeShip->Play();
                    return;
                }
                
            }

        }
    }

}/* ==== End of: void CollisionDetection( void )  ========================= */




// ---------------------------------------------------------------------------
//
// Name:         DisplayStatistics
//
// Function:     Displays Score, Hi-Score and number of lives.
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void DisplayStatistics( void )
{
	static char strScore[80];
	static char strHiScore[80];
    
    assert( Lives   > -1 );
    assert( Lives   <= 3 );
    assert( Score   >= 0 );
    assert( HiScore >= 0 );

    sprintf(strScore,   "Score:  %lu",      Score);
    sprintf(strHiScore, "Hi-Score < %lu >", HiScore);

    
    HDC dc = Back->GetDC();
    UINT Default = GetTextAlign(dc);                // Get default text alignment

    Back->TextXY(0,  0, SCORE_COLOR, strScore);      //Display score on the far left

	SetTextAlign(dc, TA_CENTER | VTA_TOP);
    Back->TextXY( ScreenWidth/2, 0, HI_SCORE_COLOR, strHiScore); //Display HiScore in the middle

    SetTextAlign(dc, Default);                      //Restore default
   
    Back->ReleaseDC();
    
    if( Lives >= 1)
    {
        LivesSSprite->SetPos( ScreenWidth-25, -5 );	
        LivesSSprite->Draw( Back , 0 , 0, CDXBLT_TRANSSCALED );
    }

    if( Lives >= 2)
    {
        LivesSSprite->SetPos( ScreenWidth-41, -5 );	
        LivesSSprite->Draw( Back , 0 , 0, CDXBLT_TRANSSCALED );
    }

    if( Lives ==3 )
    {
        LivesSSprite->SetPos( ScreenWidth-57, -5 );	
        LivesSSprite->Draw( Back , 0 , 0, CDXBLT_TRANSSCALED );
    }
    
}/* ==== End of: void DisplayStatistics( void )  ========================== */



// ---------------------------------------------------------------------------
//
// Name:        PauseGame  
//
// Function:    Freeze game!
//              GameState function.     GameState = PAUSE
//                  
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void PauseGame( void ) 
{
    Back->Fill(0);
    DrawObjects();
    DisplayStatistics();

    HDC dc = Back->GetDC(); 
    UINT DefaultMode = GetTextAlign(dc);
    SetTextAlign(dc, TA_CENTER | VTA_CENTER);
    Back->SetFont(); //??????????????????????????????????? what font
    Back->TextXY(ScreenWidth/2, ScreenHeight/2, RED, "P A U S E D"); // TextXY(x, y, color, string)
    SetTextAlign(dc, DefaultMode);
    Back->ReleaseDC();

    Screen->Flip();

}/* ==== End of: void PauseGame( void )  ============================== */


// ---------------------------------------------------------------------------
//
// Name:        NextLevel    
//
// Function:    Display game leve and get player ready. This is a GameState function.
//              GameState = NEXT_LEVEL
//                  
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void NextLevel( void )
{

	char strLevel[24]; 
	char strTime[16];
	int ElapsedTime = 3;
	DWORD PC;

	Back->Fill(0);
	
	/***
    Fire the Sky !!!!!
    ***/
	if( PCNextLevel->GetValue() >= NEXT_LEVEL_DELAY )
	{
		PCShip->Reset(); // Reset Ship counter
		PCStars->Reset();
		PCFireBullet->Reset();
		PCBullet->Reset();
		PCSIMove->Reset();
		PCSIExplosion->Reset();
		PCShipExplode->Reset();
		PCUFOExplosion->Reset();
		PCBulletExplode->Reset();
		
		GameState = PLAY_GAME;
		return;
	}

	// setup variables
	PC = PCNextLevel->GetValue();
	if( PC >= 1000 && PC < 2000)
		ElapsedTime = 2;
	else
	if( PC >= 2000 )
		ElapsedTime = 1;

	sprintf(strTime, "%d", ElapsedTime);
	sprintf(strLevel, "L e v e l   %d", GameLevel);

	// draw text
	HDC dc = Back->GetDC(); 
    UINT DefaultMode = GetTextAlign(dc);
    SetTextAlign(dc, TA_CENTER | VTA_CENTER);
    Back->TextXY(ScreenWidth/2, ScreenHeight/2, GREEN, strLevel); // TextXY(x, y, color, string)
	Back->TextXY(ScreenWidth/2, ScreenHeight/2+40, YELLOW, strTime); 
    SetTextAlign(dc, DefaultMode);
    Back->ReleaseDC();

	Screen->Flip();

	
}/* ==== End of: void NextLevel( void )  =============================== */


// ---------------------------------------------------------------------------
//
// Name:        GameOver     
//
// Function:    Displays Game Over text, stars, statistics and credits!
//              GameState function.     GameState = GAME_OVER
//                  
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void GameOver( void ) // change to this state when Lives == 0
{
    Back->Fill( 0 );

    if( PCStars->GetValue() >= STAR_DELAY )
    {
	    Stars->Update(); // Update stars
        PCStars->Reset();
    }
    else
    {
        Stars->DrawStars();
    }

    DisplayStatistics();

    // Draw the rest of the shields
    int a,b,c,d;
    Back->Lock();
    for(int i=0; i<SHIELD_HEIGHT; i++)
    {
        for(a=0; a<SHIELD_WIDTH; a++)
            if( Shield1[i][a] == 1 )
                Back->PutPixel( SHIELD_1_X+a, SHIELD_Y+i, ShieldPixel);

        for(b=0; b<SHIELD_WIDTH; b++)
            if( Shield2[i][b] == 1 )
                Back->PutPixel( SHIELD_2_X+b, SHIELD_Y+i, ShieldPixel);

        for(c=0; c<SHIELD_WIDTH; c++)
            if( Shield3[i][c] == 1 )
                Back->PutPixel( SHIELD_3_X+c, SHIELD_Y+i, ShieldPixel);

        for(d=0; d<SHIELD_WIDTH; d++)
            if( Shield4[i][d] == 1 )
                Back->PutPixel( SHIELD_4_X+d, SHIELD_Y+i, ShieldPixel);

    }
    Back->UnLock();

    CreditsFont->DrawAligned( 0 , 110 , 640 , "Credits:" , Back , CDX_CENTERJUSTIFY );
    CreditsScroll->DrawTrans( Back );
    if( PCScrollTimer->GetValue() >= SCROLL_DELAY )
    {
        CreditsScroll->ScrollUp();
        PCScrollTimer->Reset();
    }
    if( CreditsScroll->IsTopEnd() ) 
        CreditsScroll->ResetPosition();

    
    HDC dc = Back->GetDC(); 
	UINT DefaultMode = GetTextAlign(dc);
    SetTextAlign(dc, TA_CENTER | VTA_CENTER);
//	Back->SetFont(); //??????????????????????????????????? what font
	Back->TextXY(ScreenWidth/2, 60, RED, "G A M E    O V E R"); // TextXY(x, y, color, string)
    SetTextAlign(dc, DefaultMode);
	Back->ReleaseDC();
    
    Screen->Flip();


}/* ==== End of: void GameOver( void )  ============================== */



// ---------------------------------------------------------------------------
//
// Name:        CreateMenus()         
//
// Function:    Create Menus here because they have to be destroyed and    
//              recreated upon display switch.  
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void CreateMenus( void )
{
	/***
    Create OptionsMenu
    ***/
	OptionsMenu = new CDXMenu( );
	              OptionsMenu->Create( Screen->GetBack( ) , 40 );
	              OptionsMenu->SetTitle( "...[Esc] to Exit" , RGB( 0 , 255 , 0 ) );

	/***
    Set up the OptionsMenu
    ***/
    OptionsMenu->AddItem( "Switch to Full Screen Mode" , RGB( 255 , 0 , 0 ) , RGB( 255 , 255 , 255 ) );
    if(g_bFullScreen) OptionsMenu->EnableItem(0,FALSE);
	OptionsMenu->AddItem( "Switch to Window Mode" , RGB( 255 , 0 , 0 ) , RGB( 255 , 255 , 255 ) );
    if(!g_bFullScreen) OptionsMenu->EnableItem(1,FALSE);
    OptionsMenu->AddItem( "Sound OFF" , RGB( 255 , 0 , 0 ) , RGB( 255 , 255 , 255 ) );
    if( !SoundFlag ) OptionsMenu->EnableItem(2,FALSE);
    OptionsMenu->AddItem( "Sound ON" , RGB( 255 , 0 , 0 ) , RGB( 255 , 255 , 255 ) );
    if( SoundFlag ) OptionsMenu->EnableItem(3,FALSE);
    
    if(g_bFullScreen)
        OptionsMenu->SetCurrentItem(1);
    else
        OptionsMenu->SetCurrentItem(0);
	
	/***
    Create the main menu
    ***/
	MainMenu = new CDXMenu();
			   MainMenu->Create(Screen->GetBack(), MENU_ITEM_HEIGHT);
			   // Set the title of the menu to nothing
			   MainMenu->SetTitle("", RGB(0, 0, 0));
			   // Add some options to the menu
			   MainMenu->AddItem("Start Game", RGB(255, 255, 0), RGB(255, 255, 255));
			   MainMenu->AddItem("Options",    RGB(255, 255, 0), RGB(255, 255, 255));
			   MainMenu->AddItem("Quit",	   RGB(255, 255, 0), RGB(255, 255, 255));
			   MainMenu->Home();

	/***
    Change the screen font            ???????????????
    ***/
//	if( ScreenHeight == 480 )
//		Screen->GetBack()->ChangeFont("Arial", 0, 18, FW_BOLD);
//	else
//	if( ScreenHeight == 600 )
//		Screen->GetBack()->ChangeFont("Arial", 0, 20, FW_BOLD);

 //   ????????? do we have to set this font  like   Screen->GetBack()->SetFont();


} /* ==== End of: void CreateMenus( void )  =============================== */





// ===========================================================================
//  Following block of functions is window setup and
//  video mode setup
// ===========================================================================


// ---------------------------------------------------------------------------
//
// Name:        StartVideoMode        
//
// Function:    Sets Up video mode regarding to g_bFullScreen flag and try to start
//              default full screen 640x480 ?  
//              If that fails, then start app in windowed mode 
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void StartVideoMode( void )
{
	AdjustWinStyle();
	Screen->CreateBackBufferInVRAM( BackBufferInVRAM );

	if(g_bFullScreen)
	{
		// start app fullscreen with default ScreenWidth , ScreenHeight , ScreenBPP 
		if( Screen->CheckIfVideoModeExists( ScreenWidth , ScreenHeight , ScreenBPP ) == TRUE )
		{
			if(FAILED(Screen->CreateFullScreen(g_hWnd, ScreenWidth, ScreenHeight, ScreenBPP)))
			{
				g_bFullScreen = !g_bFullScreen;
				Screen->GetDD()->RestoreDisplayMode();
				AdjustWinStyle();
				if (FAILED(Screen->CreateWindowed(g_hWnd, ScreenWidth, ScreenHeight))) 
					CDXError( Screen , "Could not start default video mode " );
			}
				
		}
		else
		{
			g_bFullScreen = !g_bFullScreen;
			Screen->GetDD()->RestoreDisplayMode();
			AdjustWinStyle();
			if(FAILED(Screen->CreateWindowed(g_hWnd, ScreenWidth, ScreenHeight))) 
				CDXError( Screen , "Could not start default video mode " );	
		}
	}		
	else 
	{
		// start app in a window
		if (FAILED(Screen->CreateWindowed(g_hWnd, ScreenWidth, ScreenHeight))) 
		CDXError( Screen , "Could not start windowed mode " );
	}
}



// ---------------------------------------------------------------------------
//
// Name:        ChangeVideoMode        
//
// Function:    Switch to selected video mode
//                  
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void ChangeVideoMode( void )
{
    // before the video mode is changed, we have to destroy all surfaces
	CurrentMenu = 0;
	SAFEDELETE( MainMenu );
	SAFEDELETE( OptionsMenu );
    SAFEDELETE( CreditsFont );
    SAFEDELETE( CreditsScroll);

    /***
    Set the new video mode
	***/
    if( g_bFullScreen )
	{
		AdjustWinStyle();
		Screen->CreateBackBufferInVRAM( BackBufferInVRAM );
		Screen->SetTripleBuffering( TripleBuffer );
		if (FAILED(Screen->ChangeVideoMode(ScreenWidth , ScreenHeight , ScreenBPP )))
			CDXError (Screen, "Could not change to Full Screen");
		CDXLOG (">>> Full screen done <<<");
	}
	else
	{
		Screen->GetDD()->RestoreDisplayMode();
		AdjustWinStyle();
		Screen->CreateBackBufferInVRAM( BackBufferInVRAM );
        Screen->SetTripleBuffering( TripleBuffer );
		if (FAILED(Screen->ChangeVideoMode(ScreenWidth , ScreenHeight, 0 )))
			CDXError (Screen, "Could not set window mode");
		CenterWindow();
	}
	
    /***
    Recreate the surfaces we destroyed before
	***/
    CreateMenus( );
	CurrentMenu = OptionsMenu;
    CreditsFont = new CDXBitmapFont( Screen , "Westminster" , 22 , YELLOW );
    CreditsScroll = new CDXVertTextScroller( Screen , &CreditsClipRect ,ScrollText, CDX_CENTERJUSTIFY , 4 , CreditsFont );
    
    /***
    Set up the OptionsMenu
    ***/
    if(  g_bFullScreen ) OptionsMenu->EnableItem(0,FALSE);
    if( !g_bFullScreen ) OptionsMenu->EnableItem(1,FALSE);
    if( !SoundFlag ) OptionsMenu->EnableItem(2,FALSE);
    if(  SoundFlag ) OptionsMenu->EnableItem(3,FALSE);
    
    if(g_bFullScreen)
        OptionsMenu->SetCurrentItem(1);
    else
        OptionsMenu->SetCurrentItem(0);

	GameState = SETUP;
}


// ---------------------------------------------------------------------------
//
// Name:        CenterWindow       
//
// Function:    centers the window on the screen 
//                  
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
void CenterWindow()
{
	RECT r;
	int sx, sy;

	GetWindowRect(g_hWnd, &r);

	sx = GetSystemMetrics(SM_CXFULLSCREEN);
	sy = GetSystemMetrics(SM_CYFULLSCREEN);

	SetWindowPos(g_hWnd, NULL, (sx-(r.right-r.left))/2, (sy-(r.bottom-r.top))/2, 0,
		0, SWP_NOSIZE | SWP_NOZORDER |
		SWP_NOACTIVATE);
}


// ---------------------------------------------------------------------------
//
// Name:        AdjustWinStyle         
//
// Function:    adjusts the window style according to the mode  
//                  
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
 void AdjustWinStyle()
{
	if (g_bFullScreen)  // In fullscreen mode the window must cover the whole desktop
	{
		DWORD       dwStyle;

		// Change window attributes

		dwStyle = WS_POPUP | WS_VISIBLE;

		SetWindowLong(g_hWnd, GWL_STYLE, dwStyle);

		SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXFULLSCREEN),
			GetSystemMetrics(SM_CYFULLSCREEN), SWP_NOACTIVATE |
			SWP_NOZORDER);
	}
	else
	{
        RECT        rect = {0, 0, ScreenWidth, ScreenHeight};
		DWORD       dwStyle;

		// Change window attributes

		dwStyle = GetWindowStyle(g_hWnd);
		dwStyle &= ~WS_POPUP;
		dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

		SetWindowLong(g_hWnd, GWL_STYLE, dwStyle);

		// Resize the window so that the client area is 640x480

		AdjustWindowRectEx(&rect, GetWindowStyle(g_hWnd), GetMenu(g_hWnd) != NULL,
			GetWindowExStyle(g_hWnd));

		// Just in case the window was moved off the visible area of the
		// screen.

		SetWindowPos(g_hWnd, NULL, 0, 0, rect.right-rect.left,
			rect.bottom-rect.top, SWP_NOMOVE | SWP_NOZORDER |
			SWP_NOACTIVATE);

		SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
  			SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

		CenterWindow();
	}
}



// ---------------------------------------------------------------------------
//
// Name:        WinProc          
//
// Function:    handles application messages  
//
// ---------------------------------------------------------------------------

DWORD PauseDelay;

static long PASCAL WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    
	switch(message)
	{
		case WM_ACTIVATEAPP:    g_IsAppActive = wParam;     // set if app is active or not
			                    return 0;

        case WM_CHAR:
			if(wParam == (int) '3' )
			{
				if( BulletsToFire==1 )
				{
					if(GameLevel>= 3  || DebugFlag )
						BulletsToFire = 3;
				}
				else
					BulletsToFire = 1;
			}
            return 0;


		case WM_KEYDOWN:	switch(wParam)
                            {
                                case VK_PAUSE:
                                    if( GameState == PLAY_GAME )
                                    {   
                                        UFOSound->Stop();
		                                GameState = PAUSE; 
                                	    PCShip->Pause(); 
                                        PCStars->Pause();
	                                    PCBullet->Pause();
	                                    PCSIMove->Pause();
                                        PCGameOver->Pause();
                                        PCFireBullet->Pause();
	                                    PCSIExplosion->Pause();
                                        PCShipExplode->Pause();
                                        PCUFOExplosion->Pause();
                                        PCFrameDuration->Pause();
                                        PCBulletExplode->Pause();
                                        PauseDelay = timeGetTime();
                                    }
                                    if( (GameState == PAUSE) && ((timeGetTime()-PauseDelay)>1000))
                                    {
                                        PCShip->Resume(); 
                                        PCStars->Resume();
	                                    PCBullet->Resume();
	                                    PCSIMove->Resume();
                                        PCGameOver->Resume();
                                        PCFireBullet->Resume(); 
    	                                PCSIExplosion->Resume();
                                        PCShipExplode->Resume();
                                        PCUFOExplosion->Resume();
                                        PCFrameDuration->Resume();
                                        PCBulletExplode->Resume();
                                        GameState = PLAY_GAME;
                                    }
                                    break;

			                    case VK_ESCAPE:         // if ESC key is hit, quit program
									if( GameState == INTRO )
										SendMessage(hWnd, WM_CLOSE, 0, 0);
									else
									if( GameState == SPLASH )
										GameState = INTRO;
									else
									if( GameState == SETUP )
									{
										CurrentMenu = MainMenu;
										GameState = INTRO;
									}
									break;

								case VK_UP:
									if( GameState == INTRO || GameState == SETUP )
										CurrentMenu->Up();
									break;

								case VK_DOWN:
									if( GameState == INTRO || GameState == SETUP )
										CurrentMenu->Down();
									break;
				
								case VK_RETURN:
									if( GameState == INTRO || GameState == SETUP  )
									{
										if( GameState == SETUP )
										{
                                            switch(CurrentMenu->Enter()) 
										    {
											    case 0: //Full screen
                                                    g_bFullScreen = TRUE;
												    ChangeVideoMode();
									                break;

											    case 1: //Window mode
						                            g_bFullScreen = FALSE;
												    ChangeVideoMode();
									                break;

											    case 2: //sound off
                                                    {
												        SoundFlag = FALSE;
                                                        OptionsMenu->EnableItem(2,FALSE);
                                                        OptionsMenu->EnableItem(3,TRUE);
                                                        OptionsMenu->SetCurrentItem(3);
                                                    }
												    break;

                                                case 3: //sound oN
                                                    {
												        SoundFlag = TRUE;
                                                        OptionsMenu->EnableItem(2,TRUE);
                                                        OptionsMenu->EnableItem(3,FALSE);
                                                        OptionsMenu->SetCurrentItem(2);
                                                    }
												    break;

											    default:
												    break;
										    }
										}
										else
										switch(CurrentMenu->Enter()) 
										{
											case 0: //Play
												if( GameState == INTRO )
													GameState = RESET;
												break; 

											case 1: //setup
												CurrentMenu = OptionsMenu;
												GameState = SETUP;
												break;

											case 2: //Quit
												PostMessage(hWnd, WM_CLOSE, 0, 0);
												break;

											default:
												break;
										}
                                    }

			                }// end switch(wParam)

                            if( GameState == GAME_OVER )
                            {
                                GameState = INTRO;
                                // Save HiScore 
	                            if( Score > HiScore )
                                {
		                            WriteHighScore( Score );
                                }
                            
                                GameLevel = 0;
                                Score = 0;
                                Lives = 3;
                            }
			                return  0;

		case WM_SYSKEYUP:	if (wParam == VK_RETURN)    // Alt+Enter switches modes
			                {
								if( GameState == SETUP )
								{
									g_bFullScreen = !g_bFullScreen;
									ChangeVideoMode();
									return 0;
								}
                            }
                            return 1;
		
		case WM_SETCURSOR:  SetCursor(NULL);    // hide the mouse cursor
                			return 1;
		case WM_CLOSE:		cdx_DeInit();

		case WM_DESTROY:	PostQuitMessage(0);
		                    return 0;

		default:        	return DefWindowProc(hWnd, message, wParam, lParam);
	}
}



// ---------------------------------------------------------------------------
//
// Name:        ChangeToEXEDir          
//
// Function:    sets CWD to the DIR the EXE is in  
//                  
//
// Parameters:      None. 
//
// Return value:    None.
//
// ---------------------------------------------------------------------------
static void ChangeToEXEDir()
{
	char buf[MAX_PATH];
	char *cptr;

	//now change the directory
	//to make sure were accessing the proper file
	GetModuleFileName(NULL, buf, MAX_PATH);

	//move pointer to end of string
	cptr = buf + lstrlen(buf);

	//find the end of the path
	do
	{
		cptr--;
	} while (*cptr != '\\');
	cptr++;
	*cptr='\0';

	//change directory
	SetCurrentDirectory(buf);

}



// ---------------------------------------------------------------------------
//
// Name:        InitApp       
//
// Function:    Create the window and the CDX objects
//                  
//
// Parameters:      int nCmdShow
//
// Return value:    BOOL.
//
// ---------------------------------------------------------------------------
static BOOL InitApp(int nCmdShow)
{
	WNDCLASS WndClass;

	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WinProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = g_hInst;
	WndClass.hIcon = LoadIcon(g_hInst, "APPICON");
	WndClass.hCursor = LoadCursor(0, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.lpszMenuName = 0;
	WndClass.lpszClassName = szClassName;
	RegisterClass(&WndClass);

	g_hWnd = CreateWindowEx(
		WS_EX_TOPMOST,
		szClassName,
		szAppName,
 		WS_POPUP,
		0,
		0,
		GetSystemMetrics(SM_CXFULLSCREEN),
		GetSystemMetrics(SM_CYFULLSCREEN),
		NULL,
		NULL,
		g_hInst,
		NULL);

    // when hWnd = -1 there was an error creating the main window
    // CDXError needs a CDXScreen object, if there is none at this early
    // program stage, pass it NULL
	if( !g_hWnd ) 
        CDXError( NULL , "could not create the main window" );

    if (!g_bFullScreen)
	AdjustWinStyle();

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return TRUE;
}


// ---------------------------------------------------------------------------
//
// Name:        WinMain       
//
// Function:    inital function called by windows
//                      
// ---------------------------------------------------------------------------
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	// save the app instance
	g_hInst = hInstance;

    // Set current directory. 
    ChangeToEXEDir(); 

    // Check last exit status
    if( ReadExitStatus() != 0x1  )
        {
            if( GetDXVersion() < 0x7 )
                MessageBox(NULL, "If you are experiencing problems with this game, get\nDirectX 7 or better from Microsoft web page !", "Space Invaderz", MB_OK | MB_ICONERROR | MB_APPLMODAL);
            else
                MessageBox(NULL, "If you are experiencing problems with this game,\ntry changing your display settings to 640x480 256 Colors !", "Space Invaderz", MB_OK | MB_ICONERROR | MB_APPLMODAL);
        }

    // Write Exit status = "FAILED"
    WriteExitStatus( 0x0 );

    // Read options
    DWORD options = ReadOptions();
    g_bFullScreen = (options & 0xff00)>>8; //first byte
    SoundFlag = options & 0xff;

	if(!InitApp(nCmdShow)) 
        CDXError( NULL , "could not initialize application" );

	if(!cdx_Init())
	{
		PostQuitMessage(0);
		return FALSE;
	}

    PCAppTiming = new CDXHPC();
    PCAppTiming->Start();

	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if(!GetMessage(&msg, NULL, 0, 0 )) return msg.wParam;
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}
		else 
        if( g_IsAppActive && ( PCAppTiming->GetValue() >= MIN_FRAME_TIME  ) )
		{
            PCAppTiming->Reset();
			cdx_DoFrame();
		}
		else
        if( ! g_IsAppActive )
		{
			WaitMessage();
		}
	}
}




/* ==== End of file _SI_C_  ==================================== */

