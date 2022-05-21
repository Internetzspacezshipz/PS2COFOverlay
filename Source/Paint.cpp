#include "Paint.h"

int Paint::Init(HWND hWND)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &Object)))
	{
		exit(1);
	}

	ZeroMemory(&Parameters, sizeof(Parameters));

	Parameters.BackBufferWidth = Width;
	Parameters.BackBufferHeight = Height;
	Parameters.Windowed = true;
	Parameters.hDeviceWindow = hWND;
	Parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Parameters.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	Parameters.BackBufferFormat = D3DFMT_A8R8G8B8;
	Parameters.EnableAutoDepthStencil = TRUE;
	Parameters.AutoDepthStencilFormat = D3DFMT_D16;
	
	Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWND, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Parameters, 0 ,&Device);
	D3DXCreateFontW(Device, 50, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Comic Sans", &Font);
	
	D3DXCreateLine(Device, &Line);

	return 0;
}

void Paint::PDrawText(char* String, int X, int Y, int A, int R, int G, int B)
{
	RECT Rect;
	Rect.top = Y;
	Rect.left = X;
	Font->DrawTextA(0, String, strlen(String), &Rect, DT_NOCLIP, D3DCOLOR_ARGB(A, R, G, B));
}

void Paint::DrawLine(int X, int Y, int H, int W, D3DCOLOR Color)
{
	int XO = X + (W / 2);
	D3DXVECTOR2 Verts[2] = { D3DXVECTOR2(XO, Y), D3DXVECTOR2(XO, Y + H) };
	Line->SetWidth(W);
	Line->Draw(Verts, 2, Color);
}

void Paint::DrawCrosshairs(int cS, int pL, int pW, D3DCOLOR Color)
{
	int CW = Width / 2;//centre width
	int CH = Height / 2;//centre height

	int pWHalf = pW / 2;//half width (for subtraction from position)
	int pLHalf = pL / 2;//half width (for subtraction from position)

	if (bDrawCenterDot)
	{
		DrawLine(CW - 1, CH - 1, 2, 2, Color);//center debug dot
	}

	DrawLine(CW + cS, CH - pWHalf, pW, pL, Color);//right
	DrawLine(CW - cS - pL, CH - pWHalf, pW, pL, Color);//Left

	DrawLine(CW - pWHalf, CH + cS, pL, pW, Color);//top
	DrawLine(CW - pWHalf, CH - cS - pL, pL, pW, Color);//bot
}

void Paint::Start()
{
	Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	Device->BeginScene();
}

void Paint::End()
{
	Device->EndScene();
	Device->PresentEx(0, 0, 0, 0, 0);
}

void Paint::Render(float CurrentCOF, char* test)
{
	//if (TargetHWND == GetForegroundWindow())
	{
		//TODO: make width and shit configurable
		DrawCrosshairs(CurrentCOF, PipLength, PipWidth, CrosshairColor);
		if (test)
		{
			PDrawText(test, Width / 2, Height / 2, 255, 255, 255, 127);
		}
	}
}

Paint::Paint(HWND OverlayHWND, int InWidth, int InHeight) 
	: Width(InWidth), Height(InHeight)
{
	Init(OverlayHWND);
}
