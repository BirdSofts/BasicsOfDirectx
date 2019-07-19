// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,17.07.2019</created>
/// <changed>ʆϒʅ,20.07.2019</changed>
// ********************************************************************************

#ifndef LEARNINGDIRECTX_H
#define LEARNINGDIRECTX_H


#pragma comment (lib, "d3d10.lib") // linkage to the 'd3d10' library
#include <windows.h>
#include <string.h>
#include <d3dcsx.h>


// defined and introduced in Windows.cpp:
extern LPCTSTR MainWindow;
extern HWND handle;
extern const int Width;
extern const int Height;

// defined and introduced in DirectX.cpp:
extern ID3D10Device* d3dDevice;
extern IDXGISwapChain* swapChain;
extern ID3D10RenderTargetView* renderTargetView;


#endif // !LEARNINGDIRECTX_H
