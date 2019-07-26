// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,27.07.2019</changed>
// ********************************************************************************

#include "LearningDirectX.h"
#include "Window.h"


LPCTSTR mainWindowName { L"The game" }; // window name
//HWND wndHandle { NULL }; // window handle
bool fullScreen { false }; // windowed


Window* wnd { nullptr };
LRESULT CALLBACK mainWndProc ( HWND handle, UINT msg, WPARAM wPrm, LPARAM lPrm )
{
  // the window procedure, needed since a member function doesn't suite wClass.lpfnWndProc
  // global window procedure forwards messages to member window procedure
  return wnd->msgProc ( handle, msg, wPrm, lPrm );
}


Window::Window ( const HINSTANCE& hInstance, const int& nShowCmd )
{
  wnd = this; // necessary to forward messages

  appInstance = hInstance;
  showCommands = nShowCmd;
  mainWindow = NULL;
  initialized = false;

  clientWidth = defaults.Width;
  clientHeight = defaults.Height;
  initial ();
};


// window initialization
void Window::initial ()
{
  try
  {
    clientWidth = settings.set ().Width;
    clientHeight = settings.set ().Height;

    // filling the instantiation of the extended version of window class,
    // a structure that handles properties and actions of a window:
    WNDCLASSEX wClass;
    wClass.cbClsExtra = 0; // extra class runtime information
    wClass.cbSize = sizeof ( WNDCLASSEX ); // the size of the structure itself (usable when passed as pointer)
    wClass.cbWndExtra = 0; // extra window runtime information
    wClass.hbrBackground = ( HBRUSH) GetStockObject ( DKGRAY_BRUSH ); // a handle to a background brush for window
    wClass.hCursor = LoadCursor ( 0, IDC_ARROW ); // the cursor for the window
    // ('LoadIcon' and 'LoadCursor' functions retrieve handles to standard common icons)
    wClass.hIcon = LoadIcon ( 0, IDI_APPLICATION ); // title bar icon
    wClass.hIconSm = LoadIcon ( 0, IDI_APPLICATION ); // taskbar icon
    wClass.hInstance = appInstance; // handle to current application instance, which has created the window
    // callback function pointer to handler of the window (default window callback procedure)
    wClass.lpfnWndProc = mainWndProc;
    // a long pointer to a constant literal representing the name of the window class itself,
    // useful when the application opens more than one window, so Windows is able to keep track of them
    wClass.lpszClassName = mainWindowName;
    // a long pointer to a constant literal representing the name of the menu to attach
    wClass.lpszMenuName = 0;
    // object style flags: below arguments are usable to redraw the window, when moving it vertically or horizontally,
    // which are not usable for a full-screen application
    wClass.style = CS_HREDRAW | CS_VREDRAW;

    // registering the window class to Windows, the function returns zero in case of failure,
    // and returns a class atom that uniquely identifies the class being registered.
    // note that since all the registered window classes by a program are unregistered,
    // when it terminates, no manual cleaning is necessary.
    if ( !RegisterClassEx ( &wClass ) )
    {
      anException.set ( "regW" );
      throw anException;
    }

    // client size: all the raw window size can't be tampered with as working area,
    // therefore the exact dimension needs to be calculated.
    // rectangle structure to store the coordinates (top-left and bottom-right)
    RECT rectangle { 0, 0, long ( clientWidth ), long ( clientHeight ) };
    if ( !AdjustWindowRectEx (
      // long pointer to the rectangle structure to be filled with the calculated coordinates
      &rectangle,
      WS_OVERLAPPEDWINDOW, // current window style (is needed to calculated the client size)
      false, // window contains a menu or not
      WS_EX_OVERLAPPEDWINDOW ) ) // current extended window style (is needed to calculated the client size)
    {
      anException.set ( "adjustW" );
      throw anException;
    } else
    {
      aLog.set ( logType::info, std::this_thread::get_id (), "mainThread", "The client window size is successfully calculated." );
      logEngineToFile.push ( aLog );
    }

    // after the properties of a window class is known to Windows, it can finally be created.
    // the below function returns a handle to the newly created window, or NULL in case of failure.
    mainWindow = CreateWindowEx (
      WS_EX_OVERLAPPEDWINDOW, // extended window style (for game NULL)
      wClass.lpszClassName, // a long pointer to a constant literal representing the registered window class name
      mainWindowName, // a long pointer to a constant literal representing the name or title of the window
      WS_OVERLAPPEDWINDOW, // window style (an overlapped window has a title bar and a border)
      // note that the below arguments can be set to default if the actual location is of no importance
      CW_USEDEFAULT, // window horizontal position in pixel (x)
      CW_USEDEFAULT, // window vertical position in pixel (y)
      // note that the below arguments can be set to default if the initial dimension is of no importance
      rectangle.right - rectangle.left, rectangle.bottom - rectangle.top, // window size
      NULL, // handle to parent or owner window (Null designates the desktop as parent)
      NULL, // handle to menu, or child-window identifier
      appInstance, // handle to current application instance, which has created the window
      NULL // pointer to window-creation data (advanced)
    );

    if ( !mainWindow )
    {
      anException.set ( "crW" );
      throw anException;
    }

    // show the window: the second parameter controls how the window is to be shown,
    // it can be passed the last parameter of the main function (nShowCmd) of the program,
    // or SW_SWOW, for example, activates the window and displays it in its current size and position.
    ShowWindow ( mainWindow, SW_SHOW );
    // using the function below, Windows is forced to update the window content,
    // additionally generating a WM_PAINT message that needs to be handled by the event handler.
    UpdateWindow ( mainWindow );
    initialized = true;
  }
  catch ( const std::exception& ex )
  {
    initialized = false;
    if ( ex.what () == "regW" )
    {
      MessageBoxA ( 0, "Window registration failed!", "Error", MB_OK | MB_ICONERROR );
      aLog.set ( logType::error, std::this_thread::get_id (), "mainThread", "Window registration failed!" );
    } else
      if ( ex.what () == "crW" )
      {
        MessageBoxA ( 0, "Window creation failed!", "Error", MB_OK | MB_ICONERROR );
        aLog.set ( logType::error, std::this_thread::get_id (), "mainThread", "Window creation failed!" );
      } else
        if ( ex.what () == "adjustW" )
        {
          MessageBoxA ( 0, "The calculation of the client window size failed!", "Error", MB_OK | MB_ICONERROR );
          aLog.set ( logType::error, std::this_thread::get_id (), "mainThread", "The calculation of the client window size failed!" );
        } else
        {
          MessageBoxA ( 0, ex.what (), "Error", MB_OK | MB_ICONERROR );
          aLog.set ( logType::error, std::this_thread::get_id (), "mainThread", ex.what () );
        }
        logEngineToFile.push ( aLog );
        initialized = false;
  }
};


// window destruction
Window::~Window ()
{
  if ( mainWindow )
    mainWindow = NULL;
};


const bool& Window::initialState ()
{
  return initialized;
};


const HWND& Window::gethHandle ()
{
  return mainWindow;
}


// the event handler, a callback function, also known as Windows Procedure,
// is called by Windows from main event loop of the system at the occurrence of an event,
// which the running window must handle, before it continues its normal flow.
// defining callback, it is an executable code, passed as an argument to another code,
// which then at some convenient time is expected to execute (call back) that argument.
// a window can handle as many or as few events as it needs,
// passing all the other ones to default Windows event handler.
// note that the number of events that an application handles,
// is in direct connection to the speed, it returns to continue its purpose.
LRESULT CALLBACK Window::msgProc (
  HWND handle, // current window handle (useful when dealing with multiple windows)
  UINT msg, // the message id that needs to be handled
  WPARAM wPrm, // additional information describing the message
  LPARAM lPrm ) // additional information describing the message
{
  switch ( msg )
  {
    case WM_KEYDOWN: // if a key is pressed
      if ( wPrm == VK_ESCAPE ) // the ESC key
      {
        if ( MessageBoxA ( 0, "Exit the program?", "Exit", MB_YESNO | MB_ICONQUESTION ) == IDYES )
        {
          //DestroyWindow ( handle ); // release the memory
          // next expression simply indicates to the system the termination intention,
          // which puts a WM_QUIT message in the message queue, subsequently causing the main event loop to bail.
          PostQuitMessage ( 0 );  // send the corresponding quite message
          running = false;
          return 0; // after despatching quit message, it is advised to return zero.
        }
      }
    case WM_DESTROY: // window is flagged to be destroyed (the close button is clicked),
      PostQuitMessage ( 0 );
      running = false;
      return 0;
  }
  // it is very important to let Window handle other for the program irrelevant messages,
  // using below pass through function, preventing the Window losing them all. :)
  return DefWindowProc ( handle, msg, wPrm, lPrm );
};
