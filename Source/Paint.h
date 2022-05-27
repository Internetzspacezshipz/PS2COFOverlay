#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//#include "imgui.h"
//#pragma comment(lib, "imgui.lib")

//#include "backends/imgui_impl_dx9.h"
//#include "backends/imgui_impl_dx9.cpp"
//#pragma comment(lib, "imgui_impl_dx9.lib")

//#include "backends/imgui_impl_win32.h"
//#include "backends/imgui_impl_win32.cpp"
//#pragma comment(lib, "imgui_impl_win32.lib")


#define USING_D3DEX 1

#if USING_D3DEX
//Typedefs to allow easier switching of type.
typedef IDirect3D9Ex D3DType;
typedef IDirect3DDevice9Ex D3DDeviceType;
#else
//Typedefs to allow easier switching of type.
typedef IDirect3D9 D3DType;
typedef IDirect3DDevice9 D3DDeviceType;
#endif

class Paint
{
	D3DType* Object = nullptr;
	D3DDeviceType* Device = nullptr;
	D3DPRESENT_PARAMETERS Parameters;
	ID3DXFont* Font = nullptr;
	ID3DXLine* Line = nullptr;

	int Width, Height;

	int Init(HWND hWND);
	void PDrawText(char* String, int X, int Y, int A, int R, int G, int B);
	void DrawLine(int X, int Y, int H, int W, D3DCOLOR Color);

	void DrawCrosshairs(int ConeSize, int CrosshairPipLength, int CrosshairPipWidth, D3DCOLOR Color);

public:
	void Start();
	void End();
	void Render(float CurrentCOF);

	//Unused currently. Might be needed if any issues with dragging
	void Clear();


	bool bDrawCenterDot = false;

	D3DDeviceType* GetDevice() const { return Device; };
	D3DType* GetD3Object() const { return Object;  };

	Paint() = delete;
	Paint(HWND OverlayHWND, int InWidth, int InHeight);
};

