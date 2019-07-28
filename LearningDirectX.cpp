// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,17.07.2019</created>
/// <changed>ʆϒʅ,28.07.2019</changed>
// ********************************************************************************

#include "LearningDirectX.h"
//#include "Window.h"
//#include "DirectX.h"
#include "Game.h"
#include "Utilities.h"


// references:
// https://bell0bytes.eu/
// https://docs.microsoft.com/
// http://www.cplusplus.com/
// http://www.rastertek.com/
// https://www.braynzarsoft.net/


bool running { true };
std::wstring gameState { L"uninitialized" };


Window* win { nullptr };
DirectX3dCore* theCore { nullptr };
theException anException;
Log aLog;
Logger<toFile> logEngineToFile;
Configuration defaults;
Configure settings;


float r { 0.0f };
float g { 0.0f };
float b { 0.0f };
int colourMod_r { 1 };
int colourMod_g { 1 };
int colourMod_b { 1 };


// main function (application entry point)
int WINAPI WinMain ( _In_ HINSTANCE hInstance, // generated instance handle by Windows for the program
                     _In_opt_ HINSTANCE hPrevInstance, // obsolete plus backward compatibility (https://bell0bytes.eu/hello-world/)
                     // a long pointer to a string, similar to the command-line parameters of the standard C/C++ main function.
                     _In_ LPSTR lpCmdLine,
                     _In_ int nShowCmd ) // indicates how the main window is to be opened (minimized, maximized)
{

  win = new Window ( hInstance, nShowCmd );
  if ( !win->initialState () )
  {
    aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", L"Window initialization failed." );
    logEngineToFile.push ( aLog );
    return EXIT_FAILURE;
  } else
  {
    aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"Window is initialized." );
    logEngineToFile.push ( aLog );
  }

  theCore = new DirectX3dCore ( hInstance );
  if ( !theCore->initialState () )
  {
    MessageBoxA ( win->gethHandle (), "The initialization of core failed.", "Error", MB_OK | MB_ICONERROR );
    aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", L"The initialization of core failed." );
    logEngineToFile.push ( aLog );
    return EXIT_FAILURE;
  } else
  {
    aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The core is initialized." );
    logEngineToFile.push ( aLog );
  }

  //lua_State* LuaState = luaL_newstate ();
  //lua_close ( LuaState );
  //aLog.set ( logType::info, std::this_thread::get_id (), "mainThread", "Lua scripting language engine is initialized." );
  //logEngineToFile.push ( aLog );

  //if ( !InitializeScene () )
  //{
  //  MessageBoxA ( win->gethHandle(), "Scene initialization failed.", "Error", MB_OK | MB_ICONERROR );
  //  aLog.set ( logType::error, std::this_thread::get_id (), "mainThread", "Scene initialization failed." );
  //  logEngineToFile.push ( aLog );
  //  return EXIT_FAILURE;
  //} else
  //{
  //  aLog.set ( logType::info, std::this_thread::get_id (), "mainThread", "Scene is initialized." );
  //  logEngineToFile.push ( aLog );
  //}

  MSG msg { 0 }; // a new message structure
  unsigned short counter { 0 };

  aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The game is initialized successfully." );
  logEngineToFile.push ( aLog );
  gameState = L"initialized";

  aLog.set ( logType::warning, std::this_thread::get_id (), L"mainThread", L"The colour is going to change constantly, pay attention to your nose!" );
  logEngineToFile.push ( aLog );
  // main part (game engine)
  do // continuous loop
  {

#pragma region peekLoop
    if ( ( counter % 500 ) == 0 )
      // check and peek (get) window messages already placed in an event queue.
      // note the difference between the below two functions:
      // the get function, once called, actually waits for a message,
      // while peek function allows the normal flow, if there is no message in the message queue.
      while ( // peek loop for messages (empting the message queue)
              PeekMessage (
                // pointer to a message structure to receive message information
                &msg,
                // handle to the window, whose messages is intended,
                // and NULL as argument allow the retrieve of all messages for any window, which belongs to the current thread.
                win->gethHandle (),
                // to retrieve messages filtered through the introduced range. (both zero retunes all available messages)
                // first message to last message
                0, 0,
                // removal flags specify how the messages are to be handled:
                // additionally to below introduced argument, PM_NOREMOVE prevents the removal of messages in the queue,
                // therefore after it is passed, the get function is additionally needed to actually retrieve the messages.
                PM_REMOVE ) )
        //while ( GetMessage ( &msg, NULL, 0, 0 ) ) // not a good one for a game, which needs to deliver 30 F/S
      {
        // translation of the virtual-key messages into character messages
        TranslateMessage ( &msg );
        // dispatching the message to the window procedure function, so the event could be handled appropriately.
        DispatchMessage ( &msg );
        // the evaluated value: exit (using intellisense or MSDN, the possible and obvious messages could be seen)
        //if ( msg.message == WM_QUIT )
        //  running = false;
      }
#pragma endregion

    if ( !win->isPaused () )
    {
      if ( theCore->pause () )
        theCore->pause () = false;

      r += colourMod_r * 0.00001f;
      g += colourMod_g * 0.00005f;
      b += colourMod_b * 0.00010f;

      if ( ( r >= 1.0f ) || ( r <= 0.0f ) )
        colourMod_r *= -1;
      if ( ( g >= 1.0f ) || ( g <= 0.0f ) )
        colourMod_g *= -1;
      if ( ( b >= 1.0f ) || ( b <= 0.0f ) )
        colourMod_b *= -1;

      float backColor [4] { r, g, b, 1.0f };

      theCore->d3dDevice->ClearRenderTargetView ( theCore->renderTargetView, backColor );

      theCore->swapChain->Present ( 0, 0 );

      counter++;
      // my environment could manage 10! :)
      if ( ( counter % 10000 ) == 0 )
      {
        std::wstring str { L"" };
        for ( unsigned char i = 0; i < 4; i++ )
        {
          if ( i == 0 )
            str += std::to_wstring ( backColor [i] );
          else
            str += L", " + std::to_wstring ( backColor [i] );
        }
        aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The colour is now: RGBA ( " + str + L" )" );
        logEngineToFile.push ( aLog );
        counter = 0;
      }
    } else
      theCore->pause () = true;

  } while ( running == true );

  theCore->shutdown ();
  std::this_thread::sleep_for ( std::chrono::milliseconds { 100 } );
  gameState = L"uninitialized";

  delete theCore;
  delete win;

  return EXIT_SUCCESS;

}


// Todo add a logger: https://bell0bytes.eu/thread-safe-logger/
