// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,17.07.2019</created>
/// <changed>ʆϒʅ,21.07.2019</changed>
// ********************************************************************************

#ifndef LEARNINGDIRECTX_H
#define LEARNINGDIRECTX_H


#pragma comment (lib, "d3d10.lib") // linkage to the 'd3d10' library
#include <windows.h>
#include <string.h>
#include <exception>
#include <d3dcsx.h>


// defined and introduced in Windows.cpp:
extern bool running;
extern LPCTSTR mainWindowName;
extern HWND handle;
extern int Width;
extern int Height;
extern bool fullScreen;

// defined and introduced in DirectX.cpp:
extern ID3D10Device* d3dDevice;
extern IDXGISwapChain* swapChain;
extern ID3D10RenderTargetView* renderTargetView;


class theException : public std::exception
{
private:
  const char* expected;
public:
  void set ( const char* );
  const char* what () const throw( );
};
extern theException anException;


#endif // !LEARNINGDIRECTX_H
