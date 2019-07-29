// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,17.07.2019</created>
/// <changed>ʆϒʅ,29.07.2019</changed>
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

  DirectX3dCore theCore ( hInstance );
  if ( !theCore.initialState () )
  {
#ifndef _NOT_DEBUGGING
    aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", L"The initialization of core failed." );
    logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING
    return EXIT_FAILURE;
  } else
  {
#ifndef _NOT_DEBUGGING
    aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The core is initialized." );
    logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING
  }

  MSG msg { 0 }; // a new message structure
  unsigned short counter { 0 };

#ifndef _NOT_DEBUGGING
  aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The game is initialized successfully." );
  logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING
  gameState = L"initialized";

#ifndef _NOT_DEBUGGING
  aLog.set ( logType::warning, std::this_thread::get_id (), L"mainThread", L"Entering the game loop: the colour is going to change constantly, pay attention to your nose!" );
  logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

  theCore.getTimer ()->event ( "reset" ); // reset (start)

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
                theCore.getHandle (),
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
      }
#pragma endregion

    // tick the timer to calculate a frame
    theCore.getTimer ()->tick ();

    if ( !theCore.pauseState () )
    {

      // a game loop purpose:
      // -- process inputted data

      // -- fps calculation
      theCore.frameStatistics ();

      // -- update the game universe/logic
      // Note the needed delta time

      // -- output a frame

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

      theCore.d3dDevice->ClearRenderTargetView ( theCore.renderTargetView, backColor );

      theCore.swapChain->Present ( 0, 0 );

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
#ifndef _NOT_DEBUGGING
        aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The colour is now: RGBA ( " + str + L" )" );
        logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING
        counter = 0;
      }

    }

  } while ( running == true );

  theCore.shutdown ();
  std::this_thread::sleep_for ( std::chrono::milliseconds { 100 } );
  gameState = L"uninitialized";

  return EXIT_SUCCESS;
}
