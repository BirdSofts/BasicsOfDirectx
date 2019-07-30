// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,30.07.2019</changed>
// ********************************************************************************

#ifndef WINDOW_H
#define WINDOW_H


#include "DirectX.h"


class Window
{
  friend class DirectX3dCore;
private:
  HINSTANCE appInstance;
  DirectX3dCore* theCore;
  HWND theHandle;
  bool initialized;
  unsigned int clientWidth;
  unsigned int clientHeight;
  bool fullScreen;
  bool minimized;
  bool maximized;
  bool resizing;
public:
  Window ( DirectX3dCore* );
  const bool& initialState ();
  const HWND& getHandle ();
  void shutdown ( void );
  virtual LRESULT CALLBACK msgProc ( HWND, UINT, WPARAM, LPARAM );
};


#endif // !WINDOW_H
