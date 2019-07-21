// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,21.07.2019</changed>
// ********************************************************************************

#ifndef WINDOW_H
#define WINDOW_H


class Window
{
private:
  HWND mainWindow;
  bool initialized;
public:
  Window ( const HINSTANCE&, const int& );
  ~Window ();
  const bool& initialState ();
  const HWND& gethHandle ();
  virtual LRESULT CALLBACK msgProc ( HWND, UINT, WPARAM, LPARAM );
};


#endif // !WINDOW_H
