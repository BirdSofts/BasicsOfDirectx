// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,26.07.2019</changed>
// ********************************************************************************

#ifndef WINDOW_H
#define WINDOW_H


#include "DirectX.h"


class Window
{
  //friend class DirectX3dCore;
private:
  HWND mainWindow;
  bool initialized;
  unsigned int clientWidth;
  unsigned int clientHeight;
public:
  Window ( const HINSTANCE&, const int& );
  ~Window ();
  const bool& initialState ();
  const HWND& gethHandle ();
  virtual LRESULT CALLBACK msgProc ( HWND, UINT, WPARAM, LPARAM );
};


#endif // !WINDOW_H
