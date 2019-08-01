// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,17.07.2019</created>
/// <changed>ʆϒʅ,01.08.2019</changed>
// ********************************************************************************

#ifndef LEARNINGDIRECTX_H
#define LEARNINGDIRECTX_H

#if WIN32
#define WIN32_LEAN_AND_MEAN // rarely-used stuff exclusion
#endif // WIN32


#include <windows.h> // Windows standard APIs
#include <d3d11.h> // Windows standard DirectX3D APIs
#pragma comment (lib, "d3d10.lib") // linkage to the 'd3d10' library
#pragma comment (lib, "d3d11.lib") // linkage to the 'd3d11' library
#include <ShlObj.h> // Windows standard control APIs
#include <Shlwapi.h> // Windows standard lightweight utility APIs
#include <PathCch.h> // Windows standard shell APIs
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "PathCch.lib")
#include <iostream> // C++ standard input and output streams
#include <cstdlib> // C++ standard general utilities
#include <string.h> // C++ standard string class
#include <tchar.h> // Microsoft string's differences coverage library
#include <locale> // C++ standard locals (facets of the below header is declared here)
#include <codecvt> // C++ standard Unicode conversion facets
#include <exception> // C++ standard exception
#include <list> // C++ standard list container
#include <sstream> // C++ standard string streams
#include <fstream> // C++ standard file streams
#include <atomic> // C++ standard atomic objects (no data races)
#include <thread> // C++ standard threads
#include <mutex> // C++ standard lockable objects

// Lua: a powerful, lightweight and embeddable scripting language engine,
// and additionally the most popular general-purpose one used in games nowadays.
// -- more information: http://luabinaries.sourceforge.net/download.html
// -- Lua documentation: https://www.lua.org/docs.html
// -- Sol documentation: https://sol2.readthedocs.io/en/latest/
// purposes: write and read configuration files, run scripts and to write code to drive the gameplay.
// Sol: the go-to framework for high-performance binding between Lua and C++
// -- more information: https://github.com/ThePhD/sol2/releases
#include <lua.hpp> // Lua scripting language engine header file
#pragma comment (lib, "liblua53.a") // Lua scripting language engine library
#define SOL_ALL_SAFETIES_ON 1
#include <sol.hpp> // Sol binding framework between Lua and C++


//#define _NOT_DEBUGGING


// LearningDirectX.cpp:
extern bool running;
extern std::wstring gameState;


#endif // !LEARNINGDIRECTX_H
