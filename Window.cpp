// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,28.07.2019</changed>
// ********************************************************************************

#include "LearningDirectX.h"
#include "Window.h"


Window* wnd { nullptr };
LRESULT CALLBACK mainWndProc ( HWND handle, UINT msg, WPARAM wPrm, LPARAM lPrm )
{
  // the window procedure, needed since a member function doesn't suite wClass.lpfnWndProc
  // global window procedure forwards messages to member window procedure
  return wnd->msgProc ( handle, msg, wPrm, lPrm );
}


Window::Window ( const HINSTANCE& hInstance, const int& nShowCmd ) :
  appInstance ( hInstance ), showCommands ( nShowCmd ), appWindow ( NULL ), initialized ( false ),
  minimized ( false ), maximized ( false ), resizing ( false ), paused ( false )
{
  wnd = this; // necessary to forward messages

  clientWidth = defaults.Width;
  clientHeight = defaults.Height;
  fullScreen = false;

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
    wClass.lpszClassName = L"The Game";
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
      aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The client window size is successfully calculated." );
      logEngineToFile.push ( aLog );
    }

    // after the properties of a window class is known to Windows, it can finally be created.
    // the below function returns a handle to the newly created window, or NULL in case of failure.
    appWindow = CreateWindowEx (
      WS_EX_OVERLAPPEDWINDOW, // extended window style (for game NULL)
      wClass.lpszClassName, // a long pointer to a constant literal representing the registered window class name
      L"The Game", // a long pointer to a constant literal representing the name or title of the window
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

    if ( !appWindow )
    {
      anException.set ( "crW" );
      throw anException;
    }

    // show the window: the second parameter controls how the window is to be shown,
    // it can be passed the last parameter of the main function (nShowCmd) of the program,
    // or SW_SWOW, for example, activates the window and displays it in its current size and position.
    ShowWindow ( appWindow, SW_SHOW );
    // using the function below, Windows is forced to update the window content,
    // additionally generating a WM_PAINT message that needs to be handled by the event handler.
    UpdateWindow ( appWindow );
    initialized = true;
  }
  catch ( const std::exception& ex )
  {
    initialized = false;
    if ( ex.what () == "regW" )
    {
      MessageBoxA ( appWindow, "Window registration failed!", "Error", MB_OK | MB_ICONERROR );
      aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", L"Window registration failed!" );
    } else
      if ( ex.what () == "crW" )
      {
        MessageBoxA ( appWindow, "Window creation failed!", "Error", MB_OK | MB_ICONERROR );
        aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", L"Window creation failed!" );
      } else
        if ( ex.what () == "adjustW" )
        {
          MessageBoxA ( appWindow, "The calculation of the client window size failed!", "Error", MB_OK | MB_ICONERROR );
          aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", L"The calculation of the client window size failed!" );
        } else
        {
          MessageBoxA ( appWindow, ex.what (), "Error", MB_OK | MB_ICONERROR );
          aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", settings.strConverter ( ex.what () ) );
        }
        logEngineToFile.push ( aLog );
        initialized = false;
  }
};


// window destruction
Window::~Window ()
{
  if ( appWindow )
    appWindow = NULL;
  if ( appInstance )
    appInstance = NULL;
  aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"Application main window class is destructed." );
  logEngineToFile.push ( aLog );
};


const bool& Window::initialState ()
{
  return initialized;
};


const HWND& Window::gethHandle ()
{
  return appWindow;
}


const bool& Window::isPaused ()
{
  return paused;
};


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
      if ( wPrm == VK_ESCAPE ) // the ESC key identification
      {
        // Note for the time being:
        if ( MessageBoxA ( appWindow, "Exit the program?", "Exit", MB_YESNO | MB_ICONQUESTION ) == IDYES )
        {
          // next expression simply indicates to the system intention to terminate the window,
          // which puts a WM_QUIT message in the message queue, subsequently causing the main event loop to bail.
          PostQuitMessage ( 0 ); // send the corresponding quite message
          running = false;
          gameState = L"shutting down";
          aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The destruction of the window class is acknowledged." );
          logEngineToFile.push ( aLog );
          return 0; // the message was useful and is handled.
        } else
          return 0; // the message was useful and is handled.
      }

      //case WM_DESTROY: // window is flagged to be destroyed (the close button is clicked)
    case WM_CLOSE: // the user tries to somehow close the application
      if ( MessageBoxA ( appWindow, "Exit the program?", "Exit", MB_YESNO | MB_ICONQUESTION ) == IDYES )
      {
        PostQuitMessage ( 0 );
        running = false;
        gameState = L"shutting down";
        aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The main window is flagged for destruction." );
        logEngineToFile.push ( aLog );
        return DefWindowProc ( appWindow, msg, wPrm, lPrm ); // so the window behaves logical! :)
      } else
        return 0;

    case WM_ACTIVATE: // if window activation state changes
      if ( gameState == L"gaming" )
        if ( ( LOWORD ( wPrm ) == WA_INACTIVE ) ) // activation flag
          paused = true; // the game is paused
        else
          paused = false; // the game is running
      return 0;

    case WM_MENUCHAR: // handling none mnemonic or accelerator key and preventing constant beeping
      // the games don't have a menu, this fact can easily be used to deceive the Windows,
      // binding this not-needed feature to close the non-existent menu.
      return MAKELRESULT ( 0, MNC_CLOSE );
      return 0;

    case WM_SIZE: // important for games in windowed mode (resizing the client size and game universe)
      if ( gameState == L"gaming" )
        if ( wPrm == SIZE_MINIMIZED ) // window is minimized
        {
          minimized = true;
          maximized = false;
          paused = true;
        } else
          if ( wPrm == SIZE_MAXIMIZED ) // window is maximized
          {
            minimized = false;
            maximized = true;
            paused = false;
          } else
            if ( wPrm == SIZE_RESTORED ) // window is restored, find the previous state:
            {
              if ( minimized )
              {
                minimized = false;
                theCore->resize ();
                paused = true;
              } else
                if ( maximized )
                {
                  maximized = false;
                  theCore->resize ();
                  paused = false;
                } else
                  if ( resizing )
                  {
                    if ( gameState == L"gaming" )
                      if ( !paused )
                        paused = true;
                    // a game window get seldom resized or dragged, even when such a case occur,
                    // constant response to so many WM_SIZE messages while resizing, dragging is pointless.
                  } else // response when resized
                  {
                    theCore->resize ();
                    if ( gameState == L"gaming" )
                      paused = false;
                  }
            }
          return 0;

    case WM_ENTERSIZEMOVE: // the edge of the window is being dragged around to resize it
      resizing = true;
      if ( gameState == L"gaming" )
        paused = true;
      return 0;

    case WM_EXITSIZEMOVE: // the dragging is finished and the window is now resized
      resizing = false;
      theCore->resize ();
      if ( gameState == L"gaming" )
        paused = false;
      return 0;

      // setting the possible minimum size of the window (the message is sent when a window size is about to changed)
    case WM_GETMINMAXINFO:
      // a pointer to the 'MINMAXINFO' structure is provided by the message parameter 'lPrm'
      ( ( MINMAXINFO*) lPrm )->ptMinTrackSize.x = defaults.Width;
      ( ( MINMAXINFO*) lPrm )->ptMinTrackSize.y = defaults.Height;
      return 0;
  }

  // it is very important to let Window handle other for the program irrelevant messages,
  // using below pass through function, preventing the Window losing them all. :)
  return DefWindowProc ( handle, msg, wPrm, lPrm );
};
