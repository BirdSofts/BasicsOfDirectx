// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,17.07.2019</created>
/// <changed>ʆϒʅ,23.07.2019</changed>
// ********************************************************************************

#ifndef LEARNINGDIRECTX_H
#define LEARNINGDIRECTX_H


#pragma comment (lib, "d3d10.lib") // linkage to the 'd3d10' library
#include <windows.h>
#include <iostream>
#include <string.h>
#include <exception>
#include <sstream>
#include <fstream>
#include <list>
#include <atomic>
#include <thread>
#include <mutex>
#include <d3dcsx.h>

#include "Logging.h"


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
extern Log aLog;


class theException : public std::exception
{
private:
  const char* expected;
public:
  theException ();
  void set ( const char* );
  const char* what () const throw( );
};
extern theException anException;


#endif // !LEARNINGDIRECTX_H
