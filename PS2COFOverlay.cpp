// PS2COFOverlay.cpp : Defines the entry point for the application.
//
#pragma once

//Base incl
#include "framework.h"
#include <string>
#include <ctime>

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
#include "Source/LoadoutConfigState.h"
#include "Source/DataLoader.h"
#include "Source/UserSettings.h"
#include "Source/UserInterface.h"

//Misc includes
#include "Source/MiscUtils.inl"

//Boost includes
#include "boost/shared_ptr.hpp"


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR Title[100] = L"Overlay";                  // The title bar text

int width, height;
HWND overlayHWND;
Paint* PaintObject = nullptr;
bool bNoDraw = false;

//Planetside uses vertical FOV.
//TODO: put player settings in a separate place

LoadoutConfigState LoadoutConfigStateObject;

/*Hooking*/
//idk why these are two different vars. TODO: check this
HHOOK hMouseHook;
HHOOK MouseHook;
LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (UserInterface::Get().bMenuOpen) // bool
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

UserSettings& LoadUserSettings()
{
	UserSettings& UserSettingsObject = UserSettings::Get();
	DataLoader& DataLoaderObject = DataLoader::Get();

	nlohmann::json UserSettingsJson = DataLoaderObject.LoadUserSettings();

	if (UserSettingsJson.empty())
	{
		//Write empty object
		UserSettingsObject.Serialize(true, UserSettingsJson);
		if (!DataLoaderObject.SaveUserSettings(UserSettingsJson))
		{
			exit(1);
		}
	}

	UserSettingsObject.Serialize(false, UserSettingsJson);
	return UserSettingsObject;
}

LoadoutConfigState LoadLoadout(std::string LoadoutName)
{
	LoadoutConfigState LoadoutConfigStateObject;
	{
		DataLoader& DataLoaderObject = DataLoader::Get();

		nlohmann::json LoadoutJson = DataLoaderObject.LoadUserLoadoutConfig(LoadoutName);

		if (LoadoutJson.empty())
		{
			LoadoutConfigStateObject.Serialize(true, LoadoutJson);
			if (!DataLoaderObject.SaveUserLoadoutConfig(LoadoutName, LoadoutJson))
			{
				exit(1);
			}
		}

		LoadoutConfigStateObject.Serialize(false, LoadoutJson);
		LoadoutConfigStateObject.LoadoutName = LoadoutName;
		LoadoutConfigStateObject.CurEq = 0;
	}
	//copy but I don't care.
	return LoadoutConfigStateObject;
}

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void			    Draw(float CrosshairSize);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	
    // Initialize global strings
    MyRegisterClass(hInstance);

	MiscUtils::GetDesktopResolution(width, height);

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



	//Load user settings.
	UserSettings& Settings = LoadUserSettings();

	const std::string BaseLoadoutName = "BaseLoadout";

	//Load initial loadout configuration.
	LoadoutConfigStateObject = LoadLoadout(BaseLoadoutName);

    // Main message loop:
	clock_t LastTick = 0;
    while (GetMessage(&msg, overlayHWND, 0, 0))
    {
		//Setup time
		clock_t Now = clock();
		const float DeltaTime = ((float)(Now - LastTick)) / CLOCKS_PER_SEC;
		LastTick = Now;

		if (GetAsyncKeyState(Settings.Keycode_Open))//open menu
		{
			bool bMenuOpen = UserInterface::Get().ToggleMenuOpen();
			//lazy
			Sleep(100);
			if (bMenuOpen)
			{
				//Set clickable
				auto extendedStyle = GetWindowLong(overlayHWND, GWL_EXSTYLE);
				SetWindowLong(overlayHWND, GWL_EXSTYLE, extendedStyle & ~WS_EX_TRANSPARENT);
			}
			else
			{
				//Set not clickable
				auto extendedStyle = GetWindowLong(overlayHWND, GWL_EXSTYLE);
				SetWindowLong(overlayHWND, GWL_EXSTYLE, extendedStyle | WS_EX_TRANSPARENT);
			}
		}

		if (GetAsyncKeyState(Settings.Keycode_Quit))//quit key
		{
			break;
		}

		if (GetAsyncKeyState(Settings.Keycode_Deactivate))//no draw key
		{
			//lazy
			Sleep(100);

			bNoDraw = !bNoDraw;
		}

		if (GetAsyncKeyState(VK_F7))//Debug state key
		{
			//lazy
			Sleep(100);

			UserInterface::Get().DrawStateDebug(LoadoutConfigStateObject.GetCurrentEquipped());
		}

		if (GetAsyncKeyState(VK_F8))//Debug state key
		{
			//lazy
			Sleep(100);

			LoadoutConfigStateObject.GetCurrentEquipped().SetFullAmmo();
		}

		float CurrentZoom = 1.f;

		TickOutput TO = LoadoutConfigStateObject.Tick(DeltaTime);

		//Simple conversion from angle to pixel size. Should be set in tick
		const float AngleToPixel = height / (UserSettings::Get().FOV / TO.CurrentZoom);

		//Max cone
		Draw(TO.CurrentConeOfFire * AngleToPixel);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	UnhookWindowsHookEx(MouseHook);
	UnregisterClass(Title, hInstance);

	nlohmann::json SavedUserSettingsJson;
	UserSettings::Get().Serialize(true, SavedUserSettingsJson);
	DataLoader::Get().SaveUserSettings(SavedUserSettingsJson);

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

void Draw(float CrosshairSize)
{
	if (PaintObject && ImGui::GetCurrentContext())
	{
		PaintObject->Start();

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame(height, width);

		ImGui::NewFrame();

		//Draw UI
		UserInterface::Get().DrawUI(LoadoutConfigStateObject);

		ImGui::EndFrame();

		if (!bNoDraw)
		{
			//RENDER CROSSHARIR
			PaintObject->Render(CrosshairSize);
		}

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

