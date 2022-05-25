// PS2COFOverlay.cpp : Defines the entry point for the application.
//
#pragma once

//Base incl
#include "framework.h"
#include <string>
#include <ctime>
#include "dwmapi.h"//for DwmExtendFrameIntoClientArea 

//Disable warnings for uninitialized variables
#pragma warning(disable: 26495)

//imgui incl
#include "ext/imgui/imgui.h"
#include "ext/imgui/imgui_impl_dx9.h"
#include "ext/imgui/imgui_impl_win32.h"
//imgui windows proc handler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#pragma warning(default: 26495)

//proj includes
#include "PS2COFOverlay.h"
#include "Source/Paint.h"
#include "Source/WeaponConfig.h"
#include "Source/DataLoader.h"

//Boost includes
#include "boost/shared_ptr.hpp"

// Get the horizontal and vertical screen sizes in pixel https://stackoverflow.com/questions/8690619/how-to-get-screen-resolution-in-c
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

boost::shared_ptr<DataLoader> DataLoaderPtr;

struct IDirect3DDevice9Ex;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR Title[100] = L"Overlay";                  // The title bar text

int width, height;
HWND overlayHWND;
Paint* PaintObject = nullptr;

//Planetside uses vertical FOV.
//TODO: put player settings in a separate place
float FOV = 74.f;
bool bToggleCrouch = true;

bool bMenuOpen = false;

void UpdateOverlayState(bool MenuOpen, HWND ownd)
{
	//We call this function only when we open or close the menu
	long style = GetWindowLong(ownd, GWL_EXSTYLE);
	if (MenuOpen)
	{
		style &= ~WS_EX_LAYERED;
		SetWindowLong(ownd,
			GWL_EXSTYLE, style);
		SetForegroundWindow(ownd);
	}
	else
	{
		style |= WS_EX_LAYERED;
		SetWindowLong(ownd,
			GWL_EXSTYLE, style);
	}
	//Credits go to someone from UC
}


//TODO: fix this shit because I don't think it'll work properly with an overlay type application. Is used for IMGUI
HHOOK hMouseHook;
LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (bMenuOpen) // bool
	{
		ImGuiIO& io = ImGui::GetIO();
		MOUSEHOOKSTRUCT* pMouseStruct = (MOUSEHOOKSTRUCT*)lParam;
		if (pMouseStruct != NULL) {
			switch (wParam)
			{
			case WM_LBUTTONDOWN:
				io.MouseDown[0] = true;
				break;
			case WM_LBUTTONUP:
				io.MouseDown[0] = false;
				io.MouseReleased[0] = true;
				break;
			case WM_RBUTTONDOWN:
				io.MouseDown[1] = true;
				break;
			case WM_RBUTTONUP:
				io.MouseDown[1] = false;
				io.MouseReleased[1] = true;
				break;
			}
		}
	}
	return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}
/*Hooking*/
HHOOK MouseHook;

//TODO: draw UI for adjusting weapon stats.
void InitGUI(D3DDeviceType* Device, HWND* TargetHWND)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
	//io.DisplaySize = { (float)height, (float)width };

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(TargetHWND);
    ImGui_ImplDX9_Init(Device);

	//hack code.
	MouseHook = SetWindowsHookEx(WH_MOUSE, mouseProc, 0, GetCurrentThreadId()); // Our MouseInput Hook

	ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Arial.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
}



//Current state
float CurrentCOF_ZoomAdjusted = 0.f;
bool PressedMoveKeys[4];

WeaponConfigState CurrentState;

//primary weapon
WeaponConfig Config1;
//secondary weapon
WeaponConfig Config2;
//tert weapon (rocket launcher, etc)
WeaponConfig Config3;


/* from WinUser.h L#532
 * VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
 * 0x3A - 0x40 : unassigned
 * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
 */

bool IsMoving()
{
    short MovingKeys[8]
    {
        0x41,//A
        0x61,//a

        0x44,//D
        0x64,//d

        0x53,//S
        0x73,//s

        0x57,//W
        0x77,//w
    };

    for (short i = 0; i < 8; i++)
    {
        if (GetAsyncKeyState(MovingKeys[i]))
        {
            return true;
        }
    }
    return false;
}

int calls = 0;

bool bCurrentlyCrouched = false;
bool bLastCrouchPressed = false;

//@todo: make this work
bool IsCrouching()
{
	//C = 0x43
	//c = 0x63

	bool bCPressed = GetAsyncKeyState(0x43) || GetAsyncKeyState(0x63);
	if (bToggleCrouch)
	{
		if (bLastCrouchPressed != bCPressed)
		{
			bCurrentlyCrouched = !bCurrentlyCrouched;
			bLastCrouchPressed = bCPressed;
		}
	}
	return bCPressed;
}

bool MaybeSwitchWeapon()
{
	bool bSwitchOccurred = false;

	//Weapon switching.
	if (GetAsyncKeyState(0x31))//1 key
	{
		bSwitchOccurred = true;
		CurrentState.Config = &Config1;
	}
	else if (GetAsyncKeyState(0x32))//2 key
	{
		bSwitchOccurred = true;
		CurrentState.Config = &Config2;
	}
	else if (GetAsyncKeyState(0x33))//3 key
	{
		bSwitchOccurred = true;
		CurrentState.Config = &Config3;
	}

	return bSwitchOccurred;
}


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void			    Draw();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	DataLoaderPtr = boost::shared_ptr<DataLoader>(new DataLoader());
	
    // Initialize global strings
    MyRegisterClass(hInstance);

	GetDesktopResolution(width, height);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    PaintObject = new Paint(overlayHWND, width, height);

	//Moved from end of InitInstance()
	ShowWindow(overlayHWND, nCmdShow);
	UpdateWindow(overlayHWND);

    //HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PS2COFOVERLAY));
	InitGUI(PaintObject->GetDevice(), &overlayHWND);
    MSG msg;

	SetWindowPos(overlayHWND, HWND_TOPMOST, 0, 0, width, height, 0);

	clock_t LastTick = 0;

	//todo: load configs

	CurrentState.Config = &Config1;

    // Main message loop:
    while (GetMessage(&msg, overlayHWND, 0, 0))
    {
		//Setup time
		clock_t Now = clock();
		const float DeltaTime = ((float)(Now - LastTick)) / CLOCKS_PER_SEC;
		LastTick = Now;

		//do weapon switch if it's needed.
		//todo: add support for combination weapons (UBGLs/UBSGs/etc)
		//todo: add support for special abilities altering currently equipped weapon.
		//todo: add support for equip times.
		MaybeSwitchWeapon();

		//todo: add quit button
		const bool bMoving = IsMoving();

		if (GetAsyncKeyState(VK_F3))//open menu
		{
			//lazy
			bMenuOpen = !bMenuOpen;
			Sleep(100);
			if (bMenuOpen)
			{
				auto extendedStyle = GetWindowLong(overlayHWND, GWL_EXSTYLE);
				SetWindowLong(overlayHWND, GWL_EXSTYLE, extendedStyle & ~WS_EX_TRANSPARENT);
			}
			else
			{
				auto extendedStyle = GetWindowLong(overlayHWND, GWL_EXSTYLE);
				SetWindowLong(overlayHWND, GWL_EXSTYLE, extendedStyle | WS_EX_TRANSPARENT);
			}
		}

		const bool bFiring = GetAsyncKeyState(0x01);//LMB
		const bool bIsADSed = GetAsyncKeyState(0x02);//RMB
		const bool bIsReloadPressed = GetAsyncKeyState(0x52) || GetAsyncKeyState(0x72); //R/r
		const bool bIsCrouched = IsCrouching();//crouch
		//const bool bIsSprinting = GetAsyncKeyState(VK_SHIFT);//sprint doesn't really matter rn

		const float CurrentZoom = CurrentState.Tick(DeltaTime, bFiring, bMoving, bIsCrouched, bIsADSed, bIsReloadPressed);

		//Simple conversion from angle to pixel size. Should be set in tick
		const float AngleToPixel = height / (FOV / CurrentZoom);

		//Max cone
		CurrentCOF_ZoomAdjusted = CurrentState.CurrentCOF * AngleToPixel;

		Draw();

		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	UnhookWindowsHookEx(MouseHook);
	UnregisterClass(Title, hInstance);

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = 0;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = CreateSolidBrush(RGB(0,0,0));
    wcex.lpszMenuName   = Title;
    wcex.lpszClassName  = Title;
	wcex.hIconSm		= 0;

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable
   overlayHWND = CreateWindowExW(
	   WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
	   Title,
	   Title,
	   WS_POPUP,
		1,
		1,
	   width,
	   height,
	   nullptr,
	   nullptr,
	   hInstance,
	   nullptr);

   if (!overlayHWND)
   {
	   return FALSE;
   }

   SetLayeredWindowAttributes(overlayHWND, RGB(0, 0, 0), 0, LWA_COLORKEY);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui::GetCurrentContext())
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
	}

    switch (message)
    {
    case WM_PAINT:
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


void Draw()
{
	if (PaintObject && ImGui::GetCurrentContext())
	{
		PaintObject->Start();

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame(height, width);

		ImGui::NewFrame();

		//UI GOES HERE
		if (bMenuOpen)
		{
			ImGui::Begin("Test", &bMenuOpen);
			ImGui::SetWindowSize({ 500.f, 1000.f });

			//TEST SHIT.
			ImGui::Button("ButtonName", ImVec2(500, 500));
			ImGui::Text("WHATEVER TEST SHIT THIS IS");

			ImGui::End();
		}


		ImGui::EndFrame();

		//RENDER CROSSHARIR
		PaintObject->Render(CurrentCOF_ZoomAdjusted, nullptr/*(char*)&calls*/);

		ImGui::Render();

		if (auto* DrawData = ImGui::GetDrawData())
		{
			//This is hack maybe not work right if this done.
			//DrawData->DisplaySize = ImVec2(width, height);
			ImGui_ImplDX9_RenderDrawData(DrawData);
		}

		PaintObject->End();
	}
}

