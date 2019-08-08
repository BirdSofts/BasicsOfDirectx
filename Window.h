// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,08.08.2019</changed>
// ********************************************************************************

#ifndef WINDOW_H
#define WINDOW_H


#include <windows.h> // Windows standard APIs


#include "Core.h"


// Windows OS window class container
class Window
{
  friend class TheCore;
private:
  HINSTANCE appInstance; // handle to the application instance
  HWND handle; // handle to game window (main window)
  TheCore* core; // pointer to the application core
  bool initialized; // true if the initialization was successful
  unsigned int clientWidth; // desired window resolution
  unsigned int clientHeight;
  bool fullScreen; // true if now windowed
  bool minimized; // true if minimized
  bool maximized; // true if maximized
  bool resizing; // true while being dragged around
public:
  Window ( TheCore* ); // constructor and initializer
  const bool& isInitialized (); // get the initialized state
  const HWND& getHandle (); // get the handle to main window
  void shutdown ( void ); // destruction preparations
  virtual LRESULT CALLBACK msgProc ( HWND, UINT, WPARAM, LPARAM ); // Windows callback procedure
};


#endif // !WINDOW_H
