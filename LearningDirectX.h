// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,17.07.2019</created>
/// <changed>ʆϒʅ,24.07.2019</changed>
// ********************************************************************************

#ifndef LEARNINGDIRECTX_H
#define LEARNINGDIRECTX_H


#pragma comment (lib, "d3d10.lib") // linkage to the 'd3d10' library
#include <windows.h> // Windows standard API's
#include <iostream> // C++ standard input and output streams
#include <cstdlib> // C++ standard general utilities
#include <string.h> // C++ standard string class
#include <tchar.h> // Microsoft string's differences coverage library
#include <exception> // C++ standard exception
#include <sstream> // C++ standard string streams
#include <fstream> // C++ standard file streams
#include <list> // C++ standard list container
#include <atomic> // C++ standard atomic objects (no data races)
#include <thread> // C++ standard threads
#include <mutex> // C++ standard lockable objects
#include <d3dcsx.h> // Windows standard DirectX3D API's

#include "Utilities.h"


// defined and introduced in LearningDirectX.cpp:
extern bool running;


// defined and introduced in Windows.cpp:
extern LPCTSTR mainWindowName;
extern HWND handle;
extern int Width;
extern int Height;
extern bool fullScreen;


// utilities.h:
extern theException anException;
extern Log aLog;
extern Logger<toFile> logEngineToFile;


// defined and introduced in DirectX.cpp:
//extern ID3D10Device* d3dDevice;
//extern IDXGISwapChain* swapChain;
//extern ID3D10RenderTargetView* renderTargetView;


#endif // !LEARNINGDIRECTX_H
