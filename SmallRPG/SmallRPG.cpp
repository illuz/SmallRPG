// SmallRPG.cpp : 定义应用程序的入口点。
//

#include "SmallRPG.h"

void NewGame();

BOOL GameInitialize(HINSTANCE hInstance)
{
	// Create the game engine
	g_pGame = new GameEngine(hInstance, TEXT("SimpleRPG"),
		TEXT("SimpleRPG"), IDI_SMALLRPG, IDI_SMALL, 800, 600);
	if (g_pGame == NULL)
		return FALSE;

	// Set the frame rate
	g_pGame->SetFrameRate(30);

	// Initialize the joystick
	g_pGame->InitJoystick();

	// Store the instance handle
	g_hInstance = hInstance;

	return TRUE;
}

void GameStart(HWND hWindow)
{
	// Create the offscreen device context and bitmap
	g_hOffscreenDC = CreateCompatibleDC(GetDC(hWindow));
	g_hOffscreenBitmap = CreateCompatibleBitmap(GetDC(hWindow),
		g_pGame->GetWidth(), g_pGame->GetHeight());
	SelectObject(g_hOffscreenDC, g_hOffscreenBitmap);

	// Create the scrolling background and landscape layer
	HDC hDC = GetDC(hWindow);
	g_pBackgroundLayer = new ScrollingBackground(256, 256);

	// Menu
	g_pMenuBitmap = new Bitmap(hDC, IDB_MENU, g_hInstance);
	RECT rcBounds = { 0, 0, 800, 600 };
	g_pMenuSprite = new Sprite(g_pMenuBitmap, rcBounds, BA_STOP);
	g_pMenuSprite->SetNumFrames(1);
	g_pMenuSprite->SetPosition(0, 0);

	g_pCityLayer = new BackgroundLayer(hDC, IDB_CITY, g_hInstance);
	g_pBattleLayer = new BackgroundLayer(hDC, IDB_BATTLEFIELD, g_hInstance);
	g_pBattleBackLayer = new BackgroundLayer(hDC, IDB_BATTLEFIELDBACKLAYER, g_hInstance);

	// Create and load the person bitmap
	g_pPlayerBitmap = new Bitmap(hDC, IDB_PLAYER, g_hInstance);

	// Create the AI sprites
	g_pAIBitmap = new Bitmap(hDC, IDB_AI, g_hInstance);

	// Play the background music
	g_pGame->PlayMIDISong(TEXT("Music.mid"));

	g_hScoreFont = CreateFont(30, 0, 0, 0, FW_THIN, false, false, false,
		GB2312_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_MODERN, "Microsoft YaHei");

	g_hBloodFont = CreateFont(16, 0, 0, 0, FW_THIN, false, false, false,
		GB2312_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_MODERN, "Microsoft YaHei");

	g_hMessageFont = CreateFont(30, 0, 0, 0, FW_THIN, false, false, false,
		GB2312_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_MODERN, "Microsoft YaHei");

	g_hGameoverFont = CreateFont(120, 0, 0, 0, FW_THIN, false, false, false,
		GB2312_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_MODERN, "Microsoft YaHei");

	hpen = CreatePen(PS_SOLID, 4, RGB(255, 242, 0));

	hBrush = CreateSolidBrush(RGB(255, 0, 0));

	g_iDemo = 0;
	g_iScore = 0;
	g_iblood = 100;
	g_idrop = 2;
	g_iNumLives = 4;
	g_bShowMessage = false;
	g_iSPEED = 25;
	g_bPosChanged = true;
	NewGame();
}

// record informatin into the file Data.dat
bool RecordInfo() {
	HANDLE hFile = CreateFile(TEXT("Data.dat"), GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		// The hi score file couldn't be created, so bail
		return FALSE;

	CHAR cData[25];
	//wsprintf(cData, "%05d%05d%05d%05d", g_iblood, g_idrop, g_iScore, g_iNumLives);
	sprintf_s(cData, "%05d\n%05d\n%05d\n%05d\n", g_iblood, g_idrop, g_iScore, g_iNumLives);

	DWORD dwBytesWritten;
	if (!WriteFile(hFile, &cData, 24, &dwBytesWritten, NULL)) {
		// Something went wrong, so close the file handle
		CloseHandle(hFile);
		return FALSE;
	}

	// Close the file
	return CloseHandle(hFile);
}

bool ReadInfo() {
	HANDLE hFile = CreateFile(TEXT("Data.dat"), GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		g_iblood = 100;
		g_idrop = 2;
		g_iScore = 0;
		g_iNumLives = 4;
		return FALSE;
	}

	char  cData[25];
	DWORD dwBytesRead;
	if (!ReadFile(hFile, &cData, 24, &dwBytesRead, NULL))
	{
		// Something went wrong, so close the file handle
		CloseHandle(hFile);
		return FALSE;
	}
	sscanf_s(cData, "%05d\n%05d\n%05d\n%05d\n", &g_iblood, &g_idrop, &g_iScore, &g_iNumLives);
}

void GameEnd()
{
	RecordInfo();

	// Close the MIDI player for the background music
	g_pGame->CloseMIDIPlayer();

	// Cleanup the offscreen device context and bitmap
	DeleteObject(g_hOffscreenBitmap);
	DeleteDC(g_hOffscreenDC);

	// Cleanup the bitmaps
	//delete g_pPersonBitmap;

	// Cleanup the scrolling background and landscape layer
	delete g_pBattleBackLayer;
	delete g_pCityLayer;
	delete g_pBackgroundLayer;

	// Cleanup the sprites
	g_pGame->CleanupSprites();

	// Cleanup the game engine
	delete g_pGame;
}

void GameActivate(HWND hWindow)
{
	// Capture the joystick
	g_pGame->CaptureJoystick();

	// Resume the background music
	g_pGame->PlayMIDISong(TEXT(""), FALSE);
}

void GameDeactivate(HWND hWindow)
{
	// Release the joystick
	g_pGame->ReleaseJoystick();

	// Pause the background music
	g_pGame->PauseMIDISong();
}

// 显示对话
void ShowMessage(HDC hDC, COLORREF rgb, int alpha, int x, int y, int height, int width, char *msg, HFONT font = g_hMessageFont) {
	HDC hBitmapDC = CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, height, width);
	SelectObject(hBitmapDC, hBitmap);

	// 在 hBitmapDC 里面画画
	RECT rect = { 5, 5, height - 5, width - 5 };
	FillRect(hBitmapDC, &rect, CreateSolidBrush(rgb));

	SelectObject(hBitmapDC, font);
	SetBkMode(hBitmapDC, TRANSPARENT);
	SetTextColor(hBitmapDC, RGB(0, 0, 0));
	DrawText(hBitmapDC, TEXT(msg), -1, &rect,
		DT_CENTER | DT_VCENTER);

	// 混合
	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.AlphaFormat = 0;
	bf.SourceConstantAlpha = alpha;
	AlphaBlend(hDC, x, y, height, width, hBitmapDC, 0, 0, height, width, bf);
}

void UpdateAI() {
	// g_pAISprite[0]->OffsetPosition(g_iSPEED, 0);
	int dx = g_pPlayerSprite->getX(), dy = g_pPlayerSprite->getY();
	int tx, ty, dis, speed;
	for (int i = 0; i < g_iAINum; i++) {
		if (!g_pAISprite[i]->isPlayer && !g_pAISprite[i]->IsHidden()) {
			tx = g_pAISprite[i]->getX();
			ty = g_pAISprite[i]->getY();
			dis = (tx - dx) * (tx - dx) + (ty - dy) * (ty - dy);
			if (dis < 1000) {
				g_pAISprite[i]->fight = true;
				g_iblood -= 3;
				if (g_iblood < 0) {
					g_iNumLives--;
					g_iblood = 100;
					if (g_iNumLives == 0) {
						g_bGameOver = true;
						return;
					}
				}
			}
			if (dis < 3000 && g_pPlayerSprite->fight) {
				g_pAISprite[i]->iBlood -= 8;
				if (g_pAISprite[i]->iBlood <= 0) {
					g_pAISprite[i]->SetHidden(TRUE);
					g_iScore += 100;
					continue;
				}
			}
			else {
				g_pAISprite[i]->fight = false;
			}
			if (dis < 450 * 450) {
				speed = (int)min(sqrt(dis) / 10, g_iSPEED / 2);
				if (ty > dy) {
					g_pAISprite[i]->OffsetPosition(-speed, 0);
					g_pAISprite[i]->faceDirection = true;
				}
				else {
					g_pAISprite[i]->OffsetPosition(speed, 0);
					g_pAISprite[i]->faceDirection = false;
				}
				if (tx > dx) {
					g_pAISprite[i]->OffsetPosition(0, -speed);
				}
				else {
					g_pAISprite[i]->OffsetPosition(0, speed);
				}
				g_pAISprite[i]->Action();
			}
		}
	}
}

void DrawBloodBar(HDC hDC) {
	SelectObject(hDC, g_hBloodFont);
	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, RGB(0, 0, 0));
	for (int i = 0; i < g_iAINum; i++)
		if (g_pAISprite[i]->IsHidden() == false) {
			if (g_pAISprite[i]->isPlayer)
				g_pAISprite[i]->iBlood = g_iblood;
			TCHAR szText[64];
			// blood of player
			Rectangle(hDC, g_pAISprite[i]->getY() + 19, g_pAISprite[i]->getX() - 7,
				g_pAISprite[i]->getY() + 86, g_pAISprite[i]->getX() + 3);
			RECT rect = { g_pAISprite[i]->getY() + 20, g_pAISprite[i]->getX() - 8,
				(LONG)(g_pAISprite[i]->getY() + 20 + g_pAISprite[i]->iBlood / 100.0 * 65), g_pAISprite[i]->getX() + 2 };
			FillRect(hDC, &rect, hBrush);

			rect.top -= 12;
			rect.bottom -= 10;
			rect.right += 15;
			rect.left += 15;
			wsprintf(szText, "  %d %%", g_pAISprite[i]->iBlood);
			DrawText(hDC, szText, -1, &rect,
				DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
}

void GamePaint(HDC hDC) {
	if (!g_bPosChanged) {	// only change to paint
		g_bPosChanged = false;
		return;
	}
	g_pBackgroundLayer->Draw(hDC, TRUE);

	// Draw the sprites
	g_pGame->DrawSprites(hDC);

	// Draw AIs
	if (!g_bGameOver) {
		if (g_iDemo == 1) {
			g_pPlayerSprite->Update();
			g_pPlayerSprite->Draw(hDC);
		}
		else if (g_iDemo == 2) {
			for (int i = 0; i < g_iAINum; i++) {
				g_pAISprite[i]->Update();
				g_pAISprite[i]->Draw(hDC);
			}
			DrawBloodBar(hDC);
		}

		TCHAR szText[64];
		if (g_iDemo == 0) {
			SelectObject(hDC, hpen);
			SelectObject(hDC, GetStockObject(NULL_BRUSH));
			Rectangle(hDC, 280, 300 + 70 * g_ichoosed, 480, 365 + 70 * g_ichoosed);
		}
		else {
			RECT rect = { 20, 20, 180, 70 };
			// Draw the blood bar
			// font
			wsprintf(szText, "得分：%d", g_iScore);
			SelectObject(hDC, g_hScoreFont);
			SetBkMode(hDC, TRANSPARENT);
			SetTextColor(hDC, RGB(255, 0, 0));
			DrawText(hDC, szText, -1, &rect,
				DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			// Draw information
			wsprintf(szText, "血量：%d  生命：%d  补血药：%d", g_iblood, g_iNumLives, g_idrop);
			ShowMessage(hDC,
				RGB(0, 232, 232), 400,
				50, 520, 700, 50,
				szText);

			if (g_iDemo == 1 && g_bShowMessage) {
				// Draw the message
				ShowMessage(hDC,
					RGB(0, 162, 232), 200,
					80, 380, 600, 100,
					g_sNPCMessage);
			}
		}
	}
	else {
		ShowMessage(hDC,
			RGB(255, 0, 0), 200,
			100, 100, 600, 400,
			"\nGame Over", g_hGameoverFont);
	}
}

void GameCycle() {
	// Update the sprites
	g_pGame->UpdateSprites();

	// Obtain a device context for repainting the game
	HWND  hWindow = g_pGame->GetWindow();
	HDC   hDC = GetDC(hWindow);

	// Paint the game to the offscreen device context
	GamePaint(g_hOffscreenDC);

	// Blit the offscreen bitmap to the game screen
	BitBlt(hDC, 0, 0, g_pGame->GetWidth(), g_pGame->GetHeight(),
		g_hOffscreenDC, 0, 0, SRCCOPY);

	// Cleanup
	ReleaseDC(hWindow, hDC);
}

bool AICmpFunc(PlayerSprite* a, PlayerSprite* b) {
	return a->getX() < b->getX();
}

void HandleKeys() {
	if (g_bGameOver) return;
	if (g_iInputDelay++ > 1) {
		bool pressed = false;
		if (g_iDemo == 0) {
			// menu
			if (GetAsyncKeyState(VK_RETURN) < 0) {
				if (g_ichoosed == 1) {
					g_iDemo = 1;
					NewGame();
				}
				else if (g_ichoosed == 0) {
					// TODO: Read the file
					ReadInfo();
					g_iDemo = 1;
					NewGame();
				}
				else if (g_ichoosed == 3) {
					PostQuitMessage(0);
				}
			}

			if (GetAsyncKeyState(VK_UP) < 0) {
				g_ichoosed = (g_ichoosed - 1 + 4) % 4;
			}
			if (GetAsyncKeyState(VK_DOWN) < 0) {
				g_ichoosed = (g_ichoosed + 1) % 4;
			}
		}
		else {
			// city && battle
			if (GetAsyncKeyState(VK_LEFT) < 0)
			{
				pressed = true;
				g_pPlayerSprite->faceDirection = true;

				if (g_iDemo == 2 && g_pBattleLayer->CheckLeft() && g_pPlayerSprite->getY() < 250) {
					// move background
					g_pBattleLayer->SetSpeed(g_iSPEED);
					g_pBattleLayer->SetDirection(SD_RIGHT);
					g_pBattleLayer->Update();
					g_pBattleLayer->SetSpeed(0);

					g_pBattleBackLayer->SetSpeed(g_iSPEED / 2);
					g_pBattleBackLayer->SetDirection(SD_RIGHT);
					g_pBattleBackLayer->Update();
					g_pBattleBackLayer->SetSpeed(0);

					// Let the enemies not move
					for (int i = 0; i < g_iAINum; i++) {
						if (!g_pAISprite[i]->isPlayer)
							g_pAISprite[i]->OffsetPosition(g_iSPEED, 0);
					}
				}
				else {
					g_pPlayerSprite->OffsetPosition(-g_iSPEED, 0);
				}
			}
			if (GetAsyncKeyState(VK_RIGHT) < 0) {
				pressed = true;
				g_pPlayerSprite->faceDirection = false;
				if (g_iDemo == 2 && g_pBattleLayer->CheckRight() && g_pPlayerSprite->getY() > 400) {
					// move background
					g_pBattleLayer->SetSpeed(g_iSPEED);
					g_pBattleLayer->SetDirection(SD_LEFT);
					g_pBattleLayer->Update();
					g_pBattleLayer->SetSpeed(0);
					g_pBattleBackLayer->SetSpeed(g_iSPEED / 2);
					g_pBattleBackLayer->SetDirection(SD_LEFT);
					g_pBattleBackLayer->Update();
					g_pBattleBackLayer->SetSpeed(0);

					// Let the enemies not move
					for (int i = 0; i <= 10; i++) {
						if (!g_pAISprite[i]->isPlayer)
							g_pAISprite[i]->OffsetPosition(-g_iSPEED, 0);
					}
				}
				else {
					g_pPlayerSprite->OffsetPosition(g_iSPEED, 0);
				}
			}
			if (GetAsyncKeyState(VK_UP) < 0) {
				pressed = true;
				g_pPlayerSprite->OffsetPosition(0, -g_iSPEED);
			}
			if (GetAsyncKeyState(VK_DOWN) < 0) {
				pressed = true;
				g_pPlayerSprite->OffsetPosition(0, g_iSPEED);
			}
			if (g_pPlayerSprite->fight || pressed) {
				g_pPlayerSprite->Action();
			}
			else {
				g_pPlayerSprite->Stop();
			}

			// battle
			if (g_iDemo == 1) {
				RECT r = g_pPlayerSprite->GetPosition();
				if (GetAsyncKeyState(VK_SPACE) < 0) {
					if ((r.top >= 200 && r.top <= 400) && r.left >= 680) {
						g_iDemo = 2;
						NewGame();
					}
					if ((r.top >= 200 && r.top <= 300) && (r.left >= 100 && r.left <= 200)) {
						g_bShowMessage = !g_bShowMessage;
						g_sNPCMessage = "商店\n价格：恢复(200)买补血(200)买命(500)\n按1恢复， 按2买补血，按3买命";
					}
				}
				if (g_bShowMessage) {
					// SHOP
					if (GetAsyncKeyState(0x31) < 0 && g_iScore > 200) {	// VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
						g_iblood = 100;
						g_iScore -= 200;
					}
					if (GetAsyncKeyState(0x32) < 0 && g_iScore > 200) {
						g_idrop++;
						g_iScore -= 200;
					}
					if (GetAsyncKeyState(0x33) < 0 && g_iScore > 500) {
						g_iNumLives++;
						g_iScore -= 500;
					}
				}
			}
			else {
				if (GetAsyncKeyState(VK_SPACE) < 0) {
					g_pPlayerSprite->fight = true;
					RECT r = g_pPlayerSprite->GetPosition();
					if ((r.top >= 200 && r.top <= 400) && r.left >= 680) {
						g_iDemo = 1;
						g_iScore += 1000;
						NewGame();
					}
				}
				else {
					g_pPlayerSprite->fight = false;
				}

				// Tools
				// 按2补血
				if (GetAsyncKeyState(0x32) < 0 && g_idrop > 0) {	// VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
					g_iblood = 100;
					g_idrop--;
				}
			}

			if (g_iDemo == 2) {
				UpdateAI();
				// sort AIs
				sort(g_pAISprite, g_pAISprite + 11, AICmpFunc);
			}
			g_bPosChanged = true;

			// Reset the input delay
			g_iInputDelay = 0;
		}
	}
}

void NewGame() {
	g_pGame->CleanupSprites();
	g_pBackgroundLayer->CleanLayer();

	g_iInputDelay = 0;

	if (g_iDemo == 0) {
		// Menu
		g_ichoosed = 0;
		g_pGame->AddSprite(g_pMenuSprite);
	}
	else {
		// player
		RECT rcBounds = { 0, 200, 800, 500 };
		g_pAISprite[0] = new PlayerSprite(g_pPlayerBitmap, rcBounds, BA_STOP);
		g_pPlayerSprite = g_pAISprite[0];
		g_pPlayerSprite->isPlayer = true;
		g_pPlayerSprite->SetNumFrames(5);
		g_pPlayerSprite->SetPosition(10, 300);
	}
	if (g_iDemo == 1) {
		// city
		g_bShowMessage = false;
		RECT rcViewport = { 0, 0, 800, 600 };
		g_pCityLayer->SetViewport(rcViewport);
		g_pBackgroundLayer->AddLayer(g_pCityLayer);
	}
	if (g_iDemo == 2) {
		g_bGameOver = false;

		// back
		RECT rcViewport = { 0, 0, 800, 600 };
		g_pBattleBackLayer->SetViewport(rcViewport);
		g_pBackgroundLayer->AddLayer(g_pBattleBackLayer);

		// battle
		g_pBattleLayer->SetViewport(rcViewport);
		g_pBackgroundLayer->AddLayer(g_pBattleLayer);

		// AI
		RECT rcBounds = { -4000, 200, 4000, 500 };
		g_iAINum = 11;
		for (int i = 1; i < g_iAINum; i++) {
			g_pAISprite[i] = new PlayerSprite(g_pAIBitmap, rcBounds, BA_STOP);
			g_pAISprite[i]->SetNumFrames(5);
		}
		// 1 1 2 3 3
		g_pAISprite[1]->SetPosition(600, 300);
		g_pAISprite[2]->SetPosition(600 + 800 * 1, 300);
		g_pAISprite[3]->SetPosition(600 + 800 * 2, 240);
		g_pAISprite[4]->SetPosition(600 + 800 * 2, 350);
		g_pAISprite[5]->SetPosition(550 + 800 * 3, 240);
		g_pAISprite[6]->SetPosition(550 + 800 * 3, 350);
		g_pAISprite[7]->SetPosition(620 + 800 * 3, 300);
		g_pAISprite[8]->SetPosition(550 + 800 * 4, 240);
		g_pAISprite[9]->SetPosition(550 + 800 * 4, 350);
		g_pAISprite[10]->SetPosition(620 + 800 * 4, 300);
	}
}

void MouseButtonDown(int x, int y, BOOL bLeft) { }

void MouseButtonUp(int x, int y, BOOL bLeft) { }

void MouseMove(int x, int y) { }

void HandleJoystick(JOYSTATE jsJoystickState) { }

BOOL SpriteCollision(Sprite* pSpriteHitter, Sprite* pSpriteHittee)
{
	return FALSE;
}

void SpriteDying(Sprite* pSpriteDying)
{
}
