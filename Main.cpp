// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,17.07.2019</created>
/// <changed>ʆϒʅ,17.07.2019</changed>
// ********************************************************************************

#include "Main.h"


// reference: https://www.braynzarsoft.net/

LPCTSTR MainWindow { L"windowOne" }; // window name
HWND handle { NULL }; // window handle
const int Width { 800 }; // window size
const int Height { 600 }; // window size

// functions declaration
bool initialization ( HINSTANCE hInstance, int nShowCmd, int w, int h, bool windowed ); // window initialization
int loop (); // main part (game engine)
LRESULT CALLBACK winProcedure ( HWND handle, UINT msg, WPARAM wPrm, LPARAM lPrm ); // window callback procedure

// main function (main window)
int WINAPI WinMain ( _In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR lpCmdLine,
                     _In_ int nShowCmd )
{
  if ( !initialization ( hInstance, nShowCmd, Width, Height, true ) )
  {
    MessageBox ( 0, L"Window initialization failed.", L"Error", MB_OK | MB_ICONERROR );
    return 1;
  }

  loop ();

  return 0;
}

bool initialization ( HINSTANCE hInstance, int nShowCmd, int w, int h, bool windowed )
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

int loop ()
{
  MSG msg; // a new message structure
  ZeroMemory ( &msg, sizeof ( MSG ) ); // clear the structure to zero

  while ( true ) // continuous loop
  {
    // check and peek window messages
    if ( PeekMessage ( &msg, NULL, 0, 0, PM_REMOVE ) )
    {
      if ( msg.message == WM_QUIT ) // exit
        break;

      TranslateMessage ( &msg ); // translation of the massage

      DispatchMessage ( &msg ); // sending to default window procedure
    } else
    {
      // the continuous flow
    }
  }
  return ( int) msg.wParam; // return the message
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
