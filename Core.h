// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,08.08.2019</changed>
// ********************************************************************************

#ifndef CORE_H
#define CORE_H


#include "Window.h"
#include "Timer.h"
#include "Direct3D.h"
#include "Direct2D.h"


class TheCore
{
  friend class Window;
  friend class Direct3D;
  friend class Direct2D;
private:
  HINSTANCE appInstance; // handle to the application instance
  Timer* timer; // pointer to DirectX high-precision timer application
  int fps; // frames per second
  double mspf; // render time of a frame in milliseconds
  HWND appHandle; // handle to game window (main window)
  Window* appWindow; // pointer to game window (main window)
  bool initialized; // true if the initialization was successful
  bool paused; // true if the game is paused
  bool showFPS; // if true FPS to screen (true in debug mode)
  Direct3D* d3d;
  Direct2D* d2d;
public:
  TheCore ( HINSTANCE& ); // constructor and initializer
  const bool& isInitialized ( void ); // get the initialized state
  const HINSTANCE& getInstance ( void ); // get the handle to application instance
  const HWND& getHandle ( void ); // get the handle to main window
  const bool& isPaused ( void ); // get the paused state
  Timer* getTimer ( void ); // get the pointer to application timer
  Direct3D* getd3d ( void ); // get the pointer to application Direct3D
  Direct2D* getd2d ( void ); // get the pointer to application Direct2D
  void frameStatistics ( void ); // frame statistics calculations
  void testDirect3D ( float [] ); // tester
  void shutdown ( void ); // destruction preparations
};


#endif // !CORE_H
