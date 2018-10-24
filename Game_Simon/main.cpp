/* =============================================================
INTRODUCTION TO GAME PROGRAMMING SE102

SAMPLE main

This sample illustrates how to:

1/ Implement SweptAABB algorithm between moving objects
2/ Implement a simple (yet effective) collision frame work

Key functions:
CGame::SweptAABB
CGameObject::SweptAABBEx
CGameObject::CalcPotentialCollisions
CGameObject::FilterCollision

CGameObject::GetBoundingBox

================================================================ */

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "debug.h"
#include "Game.h"
#include "GameObject.h"
#include "Textures.h"

#include "Simon.h"
#include "Brick.h"


#define WINDOW_CLASS_NAME L"SampleWindow"
#define MAIN_WINDOW_TITLE L"main"

#define BACKGROUND_COLOR D3DCOLOR_XRGB(255, 255, 200)
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define MAX_FRAME_RATE 90

#define ID_TEX_SIMON 0
#define ID_TEX_SIMON_DEATH 1
#define ID_TEX_ENEMY 10
#define ID_TEX_MISC 20
#define ID_TEX_BG 70

bool jump = false;
bool attack = false;

CGame *game;

CSIMON *simon;

vector<LPGAMEOBJECT> objects;

class CSampleKeyHander : public CKeyEventHandler
{
	virtual void KeyState(BYTE *states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
};

CSampleKeyHander * keyHandler;
DWORD start_attack = 0;
DWORD end_attack;

int start_jump;
int end_jump;

void CSampleKeyHander::OnKeyDown(int KeyCode)
{
	DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	//if (simon->GetState() == SIMON_STATE_ATTACK) {
	//	now2 = GetTickCount();
	//	// dt: the time between (beginning of last frame) and now
	//	// this frame: the frame we are about to render
	//	DWORD dt = now2 - now;
	//	int timer = 260;
	//	if (dt >= timer)
	//	{
	//		now = now2;
	//		simon->SetState(SIMON_STATE_IDLE);
	//	}
	//	else {
	//		now2 = GetTickCount();
	//		simon->SetState(SIMON_STATE_ATTACK);
	//	}
	//}
	switch (KeyCode)
	{
	case DIK_SPACE:
	case DIK_NUMPAD8:
		if (jump == true)
			break;
		start_jump = simon->GetY();
		simon->SetState(SIMON_STATE_JUMP);
			/*if (simon->GetY() > 100.f)
				simon->SetState(SIMON_STATE_JUMP);*/
		break;
	case DIK_A: // reset
		simon->SetState(SIMON_STATE_IDLE);
		//simon->SetLevel(SIMON_LEVEL_BIG);
		simon->SetPosition(50.0f, 0.0f);
		simon->SetSpeed(0, 0);
		break;
	case DIK_R :
		simon->SetState(SIMON_STATE_DIE);
		simon->SetSpeed(0, 0);
		break;
	case DIK_C :
		simon->SetState(SIMON_STATE_ATTACK);
		simon->SetSpeed(0, 0);
		break;
	}
}

void CSampleKeyHander::OnKeyUp(int KeyCode)
{
	DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);
}

void CSampleKeyHander::KeyState(BYTE *states)
{
	
	// disable control key when SIMON die 
	if (simon->GetState() == SIMON_STATE_DIE) return;
	if (simon->GetState() == SIMON_STATE_JUMP){
		end_jump = simon->GetY();
		/*	 dt: the time between (beginning of last frame) and now
		this frame: the frame we are about to render*/
		DWORD dt = start_jump - end_jump;
		if (dt >= 30)
		{
			jump = false;
			attack = false;
			simon->SetState(SIMON_STATE_IDLE);
		}
		else {
			end_jump = simon->GetY();
			jump = true;
			simon->SetAni(SIMON_ANI_JUMP_LEFT);
		}
	}
	if (simon->GetState() == SIMON_STATE_ATTACK) {		
		end_attack = GetTickCount();
	/*	 dt: the time between (beginning of last frame) and now
		 this frame: the frame we are about to render*/
		DWORD dt2 = end_attack - start_attack;
		if (dt2 >= 300)
		{
			attack = false;
			jump = false;
			simon->SetState(SIMON_STATE_IDLE);
		}
		else {
			end_attack = GetTickCount();
			attack = true;
			//simon->SetState(SIMON_STATE_ATTACK);
		}
	}
	if (game->IsKeyDown(DIK_RIGHT) || game->IsKeyDown(DIK_NUMPAD6)) {
		if (simon->GetState() != SIMON_STATE_JUMP)
			simon->SetState(SIMON_STATE_WALKING_RIGHT);
	}
	else if (game->IsKeyDown(DIK_LEFT) || game->IsKeyDown(DIK_NUMPAD4)) {
		if (simon->GetState() != SIMON_STATE_JUMP)
			simon->SetState(SIMON_STATE_WALKING_LEFT);
	}
	else if (game->IsKeyDown(DIK_Z)) {
		if (attack == true)
			return;
		start_attack = GetTickCount();
		simon->SetState(SIMON_STATE_ATTACK);
	}
	else if (simon->GetState() != SIMON_STATE_ATTACK && simon->GetState() != SIMON_STATE_JUMP)
		simon->SetState(SIMON_STATE_IDLE);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

/*
Load all game resources
In this example: load textures, sprites, animations and SIMON object

TO-DO: Improve this function by loading texture,sprite,animation,object from file
*/
void LoadResources()
{
	CTextures * textures = CTextures::GetInstance();

	textures->Add(ID_TEX_SIMON, L"textures\\SIMON.png", D3DCOLOR_XRGB(255, 0,255));
	textures->Add(ID_TEX_SIMON_DEATH, L"textures\\simondeath.png", D3DCOLOR_XRGB(255, 255, 255));
	textures->Add(ID_TEX_MISC, L"textures\\misc.png", D3DCOLOR_XRGB(255, 255, 255));
	textures->Add(ID_TEX_BG, L"textures\\state\\bg.png", D3DCOLOR_XRGB(255, 255, 255));
	/*textures->Add(ID_TEX_ENEMY, L"textures\\enemies.png", D3DCOLOR_XRGB(3, 26, 110));*/


	/*textures->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));*/


	CSprites * sprites = CSprites::GetInstance();
	CAnimations * animations = CAnimations::GetInstance();

	LPDIRECT3DTEXTURE9 texSIMON = textures->Get(ID_TEX_SIMON);

	// right
	sprites->Add(10001, 315, 202, 350, 261, texSIMON);	
	sprites->Add(10002, 378, 202, 401, 261, texSIMON);
	sprites->Add(10003, 436, 202, 468, 261, texSIMON);
	sprites->Add(10004, 378, 202, 401, 261, texSIMON);
	

	//left
	sprites->Add(10011, 136, 5, 166, 64, texSIMON);
	sprites->Add(10012, 77, 1, 101, 64, texSIMON);
	sprites->Add(10013, 12, 5, 43, 64, texSIMON);
	sprites->Add(10012, 79, 5, 101, 64, texSIMON);

	//jump right
	sprites->Add(10021, 196, 199, 228, 244, texSIMON);
	//jump left
	sprites->Add(10022, 252, 1, 284, 46, texSIMON);

	//attack left
	sprites->Add(10031, 312, 4, 359, 63, texSIMON);
	sprites->Add(10032, 373, 4, 404, 63, texSIMON);
	sprites->Add(10033, 421, 4, 463, 63, texSIMON);

	//attack right
	sprites->Add(10041, 312, 4, 359, 63, texSIMON);
	sprites->Add(10042, 373, 4, 404, 63, texSIMON);
	sprites->Add(10043, 421, 4, 463, 63, texSIMON);

	LPDIRECT3DTEXTURE9 texSIMONDEATH = textures->Get(ID_TEX_SIMON_DEATH);
	//die right
	sprites->Add(10098, 0, 6, 63, 65, texSIMONDEATH);
	//die left
	sprites->Add(10099, 64, 6, 127, 65, texSIMONDEATH);

	LPDIRECT3DTEXTURE9 texMisc = textures->Get(ID_TEX_MISC);
	//misc
	sprites->Add(20000, 0, 0, 32, 32, texMisc);


	LPDIRECT3DTEXTURE9 textBG = textures->Get(ID_TEX_BG);
	sprites->Add(20001, 0, 0, 2816, 177, textBG);

	//LPDIRECT3DTEXTURE9 texEnemy = textures->Get(ID_TEX_ENEMY);
	//sprites->Add(30001, 5, 14, 21, 29, texEnemy);
	//sprites->Add(30002, 25, 14, 41, 29, texEnemy);

	//sprites->Add(30003, 45, 21, 61, 29, texEnemy); // die sprite

	LPANIMATION ani;
	ani = new CAnimation(100);	//BG
	ani->Add(20001);
	animations->Add(700, ani);


	ani = new CAnimation(100);	// idle big right
	ani->Add(10001);
	animations->Add(400, ani);

	ani = new CAnimation(100);	// idle big left
	ani->Add(10011);
	animations->Add(401, ani);

	ani = new CAnimation(100);	//jump right
	ani->Add(10021);
	animations->Add(405, ani);

	ani = new CAnimation(100);	//jump left
	ani->Add(10022);
	animations->Add(406, ani);

	ani = new CAnimation(100);	// walk right big
	ani->Add(10001);
	ani->Add(10002);
	ani->Add(10003);
	animations->Add(403, ani);

	ani = new CAnimation(100);	// // walk left big
	ani->Add(10011);
	ani->Add(10012);
	ani->Add(10013);
	animations->Add(404, ani);

	ani = new CAnimation(100);		// SIMON die left
	ani->Add(10099);
	animations->Add(408, ani);

	ani = new CAnimation(100);		// SIMON die right
	ani->Add(10098);
	animations->Add(407, ani);

	ani = new CAnimation(100);		// attack right
	ani->Add(10041);
	ani->Add(10042);
	ani->Add(10043);
	animations->Add(409, ani);

	ani = new CAnimation(100);		// attack left
	ani->Add(10031);
	ani->Add(10032);
	ani->Add(10033);
	animations->Add(410, ani);

	ani = new CAnimation(100);		// brick
	ani->Add(20000);
	animations->Add(601, ani);

	//ani = new CAnimation(300);		// Goomba walk
	//ani->Add(30001);
	//ani->Add(30002);
	//animations->Add(701, ani);

	//ani = new CAnimation(1000);		// Goomba dead
	//ani->Add(30003);
	//animations->Add(702, ani);

	simon = new CSIMON();
	simon->AddAnimation(400);		// idle right 
	simon->AddAnimation(401);		// idle left 

	simon->AddAnimation(403);      // walk right 
	simon->AddAnimation(404);      // walk left 

	simon->AddAnimation(405);		 //jump right
	simon->AddAnimation(406);		 //jump left
	
	simon->AddAnimation(407);		// die right
	simon->AddAnimation(408);		// die left

	simon->AddAnimation(409);		// attack right
	simon->AddAnimation(410);		// attack left

	//CBrick *background = new CBrick();
	//background->AddAnimation(700);
	//background->SetPosition(0.0f, 0.47f);
	//objects.push_back(background);

	simon->SetPosition(50.0f, 0);
	objects.push_back(simon);


	/*for (int i = 0; i < 15; i++)
	{
		CBrick *brick = new CBrick();
		brick->AddAnimation(601);
		brick->SetPosition(0 + i*16.0f, 150);
		objects.push_back(brick);
	}*/
	// and Goombas 
	/*for (int i = 0; i < 4; i++)
	{
		goomba = new CGoomba();
		goomba->AddAnimation(701);
		goomba->AddAnimation(702);
		goomba->SetPosition(200 + i * 60, 135);
		goomba->SetState(GOOMBA_STATE_WALKING);
		objects.push_back(goomba);
	}*/

}

/*
Update world status for this frame
dt: time period between beginning of last frame and beginning of this frame
*/
void Update(DWORD dt)
{
	simon->Update(dt);
}

/*
Render a frame
*/
void Render()
{
	LPDIRECT3DDEVICE9 d3ddv = game->GetDirect3DDevice();
	LPDIRECT3DSURFACE9 bb = game->GetBackBuffer();
	LPD3DXSPRITE spriteHandler = game->GetSpriteHandler();

	if (d3ddv->BeginScene())
	{
		// Clear back buffer with a color
		d3ddv->ColorFill(bb, NULL, BACKGROUND_COLOR);

		spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);

		for (int i = 0; i < objects.size(); i++)
			objects[i]->Render();

		spriteHandler->End();
		d3ddv->EndScene();
	}

	// Display back buffer content to the screen
	d3ddv->Present(NULL, NULL, NULL, NULL);
}

HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;

	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	HWND hWnd =
		CreateWindow(
			WINDOW_CLASS_NAME,
			MAIN_WINDOW_TITLE,
			WS_OVERLAPPEDWINDOW, // WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			ScreenWidth,
			ScreenHeight,
			NULL,
			NULL,
			hInstance,
			NULL);

	if (!hWnd)
	{
		OutputDebugString(L"[ERROR] CreateWindow failed");
		DWORD ErrCode = GetLastError();
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

int Run()
{
	MSG msg;
	int done = 0;
	DWORD frameStart = GetTickCount();
	DWORD tickPerFrame = 1000 / MAX_FRAME_RATE;

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) done = 1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		DWORD now = GetTickCount();

		// dt: the time between (beginning of last frame) and now
		// this frame: the frame we are about to render
		DWORD dt = now - frameStart;

		if (dt >= tickPerFrame)
		{
			frameStart = now;
			game->ProcessKeyboard();
			Update(dt);
			Render();
		}
		else
			Sleep(tickPerFrame - dt);
	}

	return 1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd = CreateGameWindow(hInstance, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT);

	game = CGame::GetInstance();
	game->Init(hWnd);

	keyHandler = new CSampleKeyHander();
	game->InitKeyboard(keyHandler);


	LoadResources();

	SetWindowPos(hWnd, 0, 0, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	Run();

	return 0;
}