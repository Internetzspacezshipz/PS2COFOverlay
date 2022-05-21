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


class Paint
{
	IDirect3D9Ex* Object = nullptr;
	IDirect3DDevice9Ex* Device = nullptr;
	D3DPRESENT_PARAMETERS Parameters;
	ID3DXFont* Font = nullptr;
	ID3DXLine* Line = nullptr;

	//Red by default
	D3DCOLOR CrosshairColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	int Width, Height;


	int Init(HWND hWND);
	void PDrawText(char* String, int X, int Y, int A, int R, int G, int B);
	void DrawLine(int X, int Y, int H, int W, D3DCOLOR Color);

	void DrawCrosshairs(int ConeSize, int CrosshairPipLength, int CrosshairPipWidth, D3DCOLOR Color);

public:
	void Start();
	void End();
	void Render(float CurrentCOF, char* test);

	int PipLength = 15;
	int PipWidth = 2;

	bool bDrawCenterDot = false;

	//FLOATS ALL MUST BE 0-1 RANGE FOR THIS TO WORK PROPERLY.
	void SetColor(float R, float G, float B, float A)
	{
		CrosshairColor = D3DCOLOR_COLORVALUE(R, G, B, A);
	}

	IDirect3DDevice9Ex* GetDevice() const { return Device; };
	IDirect3D9Ex* GetObject() const { return Object;  };

	Paint() = delete;
	Paint(HWND OverlayHWND, int InWidth, int InHeight);
};

