// Small RPG

#pragma once

// Include Files
#include "stdafx.h"
#include "resource.h"
#include "GameEngine.h"
#include "Bitmap.h"
#include "Sprite.h"
#include "ScrollingBackground.h"
#include "PlayerSprite.h"
#include <algorithm>
using namespace std;



// Global Variables
HINSTANCE             g_hInstance;
GameEngine*           g_pGame;
HDC                   g_hOffscreenDC;
HBITMAP               g_hOffscreenBitmap;
int                   g_iInputDelay;


int g_iDemo;	// ³¡¾° 0:Menu 1:City 2:BattleField
bool g_bGameOver;
int g_iSPEED;

// 1.
// MENU
Bitmap* g_pMenuBitmap;
Sprite* g_pMenuSprite;
int g_ichoosed;
// TCHAR* g_sOptions[4];

// 2.
// City
BackgroundLayer* g_pCityLayer;

// 3.
// Battle Field Background
ScrollingBackground* g_pBackgroundLayer;
BackgroundLayer* g_pBattleLayer;
BackgroundLayer* g_pBattleBackLayer;

// NPC
bool g_bShowMessage;
char* g_sNPCMessage;


// AIs: Player
Bitmap* g_pAIBitmap;
Bitmap* g_pPlayerBitmap;
PlayerSprite* g_pPlayerSprite;
PlayerSprite* g_pAISprite[11];	// 0:Player 1~10:small 11:big boss
bool g_bPosChanged;
int g_iAINum;
int g_iblood;
int g_idrop;
int g_iScore;
int g_iNumLives;


// Font
HFONT g_hScoreFont;
HFONT g_hBloodFont;
HFONT g_hMessageFont;
HFONT g_hGameoverFont;
HPEN hpen;
HBRUSH hBrush;