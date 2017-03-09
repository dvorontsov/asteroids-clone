#include "asteroids-clone.h"
#include <time.h>


game_state _gameState = {};
game_state *gameState = &_gameState;

bool isGameRunning = false;

void DEBUG_DisplayAsteroidsOverlay(HDC deviceCtx)
{
	SelectObject(deviceCtx, GetStockObject(DEFAULT_GUI_FONT));
	SetBkMode(deviceCtx, TRANSPARENT);
	SetTextColor(deviceCtx, RGB(255, 255, 255));

	for (int i = 0; i < 15; i++)
	{
		int x = 10;
		int displacement = 0;
		if (i + 1 > 5 && i + 1 < 11)
		{
			x += 200;
			displacement += 5;
		}
		if (i + 1 > 10 && i + 1 < 16)
		{
			x += 400;
			displacement += 10;
		}
		int asteroidPosX = gameState->asteroids[i].position.x;
		int asteroidPosY = gameState->asteroids[i].position.y;
		char textBuffer1[50];
		int stringSize1 = sprintf_s(textBuffer1, "asteroid %d: x = %d, y = %d, state = %d",
			i, asteroidPosX, asteroidPosY, gameState->asteroids[i].state);
		TextOut(deviceCtx, x, 10 + 60 * (i - displacement), textBuffer1, stringSize1);

		char textBuffer2[50];
		int stringSize2 = sprintf_s(textBuffer2, "model memory addr: %#010x",
			gameState->asteroids[i].model);
		TextOut(deviceCtx, x, 25 + 60 * (i - displacement), textBuffer2, stringSize2);

		char textBuffer3[50];
		int stringSize3 = sprintf_s(textBuffer3, "rotational delta: %f",
			gameState->asteroids[i].model->rotationDeltaDegrees);
		TextOut(deviceCtx, x, 40 + 60 * (i - displacement), textBuffer3, stringSize2);
	}
}

void DisplayControlsText(HDC deviceCtx)
{
	HFONT hFont;
	SelectObject(deviceCtx, GetStockObject(DEFAULT_GUI_FONT));
	SetTextColor(deviceCtx, RGB(0, 0, 255));
	SetBkMode(deviceCtx, TRANSPARENT);
	hFont = CreateFont(23, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 2, 0, "SYSTEM_FIXED_FONT");
	(HFONT)SelectObject(deviceCtx, hFont);


	TextOut(deviceCtx, 30, 400, "W - forward", 11);
	TextOut(deviceCtx, 30, 420, "A  - left", 9);
	TextOut(deviceCtx, 30, 440, "S  - break", 10);
	TextOut(deviceCtx, 30, 460, "D  - right", 10);

	TextOut(deviceCtx, 200, 400, "P - pause", 9);
	TextOut(deviceCtx, 200, 420, "Z  - debug overlay on/off", 25);
	TextOut(deviceCtx, 200, 440, "X  - grid on/off", 16);

	DeleteObject(hFont);
}


void DispalyGameOverScreen(HDC deviceCtx)
{
	HFONT hFont, hFont2;
	SelectObject(deviceCtx, GetStockObject(DEFAULT_GUI_FONT));
	SetTextColor(deviceCtx, RGB(255, 0, 0));
	hFont = CreateFont(80, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "SYSTEM_FIXED_FONT");
	(HFONT)SelectObject(deviceCtx, hFont);
	SetBkMode(deviceCtx, TRANSPARENT);

	TextOut(deviceCtx, 300, 100, "GAVE OVER", 9);
	
	hFont2 = CreateFont(30, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 2, 0, "SYSTEM_FIXED_FONT");
	(HFONT)SelectObject(deviceCtx, hFont2);
	TextOut(deviceCtx, 320, 300, "Press ENTER to restart the game", 31);

	DeleteObject(hFont);
	DeleteObject(hFont2);

	DisplayControlsText(deviceCtx);
}

void DisplayStartGameScreen(HDC deviceCtx)
{
	HFONT hFont, hFont2;
	SelectObject(deviceCtx, GetStockObject(DEFAULT_GUI_FONT));
	SetTextColor(deviceCtx, RGB(0, 0, 255));
	hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "SYSTEM_FIXED_FONT");
	(HFONT)SelectObject(deviceCtx, hFont);
	SetBkMode(deviceCtx, TRANSPARENT);

	TextOut(deviceCtx, 200, 100, "ASTEROIDS CLONE", 15);

	hFont2 = CreateFont(30, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 2, 0, "SYSTEM_FIXED_FONT");
	(HFONT)SelectObject(deviceCtx, hFont2);
	TextOut(deviceCtx, 300, 200, "Press ENTER to start the game", 29);

	DeleteObject(hFont);
	DeleteObject(hFont2);

	DisplayControlsText(deviceCtx);
}


void ProcessKeyEventPress(bool isDown, bool wasDown, bool *controlState)
{
	if (isDown && !wasDown)
	{
		*controlState = true;
	}
	else if (!isDown && wasDown)
	{
		*controlState = false;
	}
}

void ProcessKeyEventToggle(bool isDown, bool wasDown, bool *controlState)
{
	if (isDown && !wasDown)
	{
		if (*controlState == true) {
			*controlState = false;
		}
		else
		{
			*controlState = true;
		}		
	}
}

LRESULT CALLBACK
WindowProc(HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	LRESULT result = 0;

	switch (msg)
	{
	case WM_CLOSE:
	{
		isGameRunning = false;
		PostQuitMessage(0);
		return 0;
	} break;

	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		bool wasDown = ((1 << 30) & lParam) != 0 ? true : false;
		bool isDown = ((1 << 31) & lParam) == 0 ? true : false;

		switch (wParam)
		{
		case 'W':
			ProcessKeyEventPress(isDown, wasDown, &gameState->controller.forward);
			break;
		case 'A':
			ProcessKeyEventPress(isDown, wasDown, &gameState->controller.left);
			break;
		case 'S':
			ProcessKeyEventPress(isDown, wasDown, &gameState->controller.back);
			break;
		case 'D':
			ProcessKeyEventPress(isDown, wasDown, &gameState->controller.right);
			break;
		case 'P':
			ProcessKeyEventToggle(isDown, wasDown, &gameState->controller.pause);
			break;
		case 'Z':
			ProcessKeyEventToggle(isDown, wasDown, &gameState->controller.debug_asteroids);
			break;
		case 'X':
			ProcessKeyEventToggle(isDown, wasDown, &gameState->controller.debug_grid);
			break;
		case VK_SPACE:
			ProcessKeyEventPress(isDown, wasDown, &gameState->controller.shoot);
			break;
		case VK_RETURN:
			ProcessKeyEventPress(isDown, wasDown, &gameState->controller.ready);
			break;

		default: 
			break;
		};
	} break;

	default:
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	}

	return result;
}

void MessageLoop()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))      //message loop
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int CALLBACK
WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	win32_video_buffer buffer = {};
	InitBuffer(&buffer, SCREEN_WIDTH, SCREEN_HEIGHT);
	srand(time(nullptr));

	WNDCLASS wc = {};

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "Asteroids Clone";
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&wc);

	// Create the window.
	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		"Asteroids Clone",              // Window class
		"Asteroids Clone",              // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Position and size
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		SCREEN_WIDTH + 16, 
		SCREEN_HEIGHT + 38,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd)
	{
		isGameRunning = true;

		InitModels(gameState->modelAssets);
		InitPlayer(gameState);
		InitAsteroids(gameState, true);
			
		ShowWindow(hwnd, nCmdShow);

		int StartTime;
		int EndTime;
		int DeltaTime;

		while (isGameRunning)
		{
			StartTime = GetTickCount();
			MessageLoop();
			HDC deviceCtx = GetDC(hwnd);
		
			ClearBuffer(&buffer, 0, 0, 0);

			if (!gameState->gameInProgress && !gameState->controller.ready)
			{
				DisplayBufferInWindow(&buffer, deviceCtx, buffer.width, buffer.height);
				DisplayStartGameScreen(deviceCtx);
			}
			else
			{
				gameState->gameInProgress = true;
				UpdateGame(gameState);

				if (gameState->controller.debug_grid)
				{
					DrawGrid(&buffer, 100);
				}

				RenderPlayer(&buffer, gameState);
				RenderAsteroids(&buffer, gameState->asteroids, ArraySize(gameState->asteroids));

				DisplayBufferInWindow(&buffer, deviceCtx, buffer.width, buffer.height);

				if (gameState->controller.debug_asteroids)
				{
					DEBUG_DisplayAsteroidsOverlay(deviceCtx);
				}

				if (gameState->isGameOver)
				{
					DispalyGameOverScreen(deviceCtx);
				}
			}
			
			ReleaseDC(hwnd, deviceCtx);


			do {
				EndTime = GetTickCount();
				DeltaTime = EndTime - StartTime;
			} while (DeltaTime < (float)1000 / FRAMES_PER_SECOND);
		}
	}

	CleanUp(gameState);
	return 0;
}