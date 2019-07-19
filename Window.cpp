// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,20.07.2019</changed>
// ********************************************************************************

#include "LearningDirectX.h"
#include "Window.h"


LPCTSTR MainWindow { L"windowOne" }; // window name
HWND handle { NULL }; // window handle
const int Width { 800 }; // window size
const int Height { 600 }; // window size


bool initializeWindow ( HINSTANCE hInstance, int nShowCmd, int w, int h, bool windowed )
{
  WNDCLASSEX winClass; // instantiation from extended version of window class
  winClass.cbSize = sizeof ( WNDCLASSEX ); // setting the exact object size from the class itself
  winClass.style = CS_HREDRAW | CS_VREDRAW; // object style
  winClass.lpfnWndProc = winProcedure; // default window callback procedure
  winClass.cbClsExtra = NULL; // extra bytes after the object structure
  winClass.cbWndExtra = NULL; // extra bytes after the object's window instance
  winClass.hInstance = hInstance; // setting the current instance
  winClass.hIcon = LoadIcon ( NULL, IDI_WINLOGO ); // title bar icon
  winClass.hCursor = LoadCursor ( NULL, IDC_ARROW ); // default mouse icon
  winClass.hbrBackground = ( HBRUSH) ( COLOR_WINDOW + 2 ); // window background colour
  winClass.lpszMenuName = NULL; // name of the attached menu to the window
  winClass.lpszClassName = MainWindow; // window name
  winClass.hIconSm = LoadIcon ( NULL, IDI_WINLOGO ); // taskbar icon

  if ( !RegisterClassEx ( &winClass ) )
  {
    MessageBox ( 0, L"Window registration failed.", L"Error", MB_OK | MB_ICONERROR );
    return 1;
  }

  // create the window and set it to the handle
  handle = CreateWindowEx (
    NULL, // extended style
    MainWindow, // window name
    L"Main Window", // window title
    WS_OVERLAPPEDWINDOW, // window style
    CW_USEDEFAULT, // window top left corner (x)
    CW_USEDEFAULT, // window top left corner (y)
    Width, Height, // windwo size
    NULL, // parent window handle
    NULL, // menu handle
    hInstance, // current instance
    NULL // MDI client window
  );

  if ( !handle )
  {
    MessageBox ( 0, L"Window creation failed.", L"Error", MB_OK | MB_ICONERROR );
    return 1;
  }

  ShowWindow ( handle, nShowCmd ); // draw the window
  UpdateWindow ( handle ); // useful update function

  return true;
}


LRESULT CALLBACK winProcedure ( HWND handle, UINT msg, WPARAM wPrm, LPARAM lPrm )
{
  switch ( msg )
  {
    case WM_KEYDOWN: // if a key is pressed
      if ( wPrm == VK_ESCAPE ) // ESC key
      {
        if ( MessageBox ( 0, L"Exit the program?", L"Exit", MB_YESNO | MB_ICONQUESTION ) == IDYES )
          DestroyWindow ( handle ); // release the memory
      }
      return 0;
    case WM_DESTROY: // window is closed by close button
      PostQuitMessage ( 0 );
      return 0;
  }
  return DefWindowProc ( handle, msg, wPrm, lPrm ); // return the received and unhandled messages to the default Windows function
}
