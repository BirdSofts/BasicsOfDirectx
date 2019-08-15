// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,17.07.2019</created>
/// <changed>ʆϒʅ,15.08.2019</changed>
// ********************************************************************************

//#include "Window.h"
//#include "Core.h"
#include "Utilities.h" // string + s,f streams + exception + threads + list + Windows standards
#include "Shared.h"
#include "Game.h"


// references:
// https://bell0bytes.eu/
// https://docs.microsoft.com/
// http://www.cplusplus.com/
// https://en.cppreference.com/
// http://www.rastertek.com/
// https://www.braynzarsoft.net/

//long long* anArray = new long long [1000000000000000000]; // throwing a standard exception on memory allocation

bool running { false };

#ifndef _NOT_DEBUGGING
bool debugger { true };
#else
bool debugger { false };
#endif // !_NOT_DEBUGGING

std::wstring gameState { L"uninitialized" };


// main function (application entry point)
int WINAPI WinMain ( _In_ HINSTANCE hInstance, // generated instance handle by Windows for the program
                     _In_opt_ HINSTANCE hPrevInstance, // obsolete plus backward compatibility (https://bell0bytes.eu/hello-world/)
                     // a long pointer to a string, similar to the command-line parameters of the standard C/C++ main function.
                     _In_ LPSTR lpCmdLine,
                     _In_ int nShowCmd ) // indicates how the main window is to be opened (minimized, maximized)
{
  try
  {

    std::shared_ptr<theException> anException { new (std::nothrow) theException () };
    PointerProvider::exceptionProvider ( anException );

#ifndef _NOT_DEBUGGING
    std::shared_ptr<Logger<toFile>> fileLoggerEngine ( new (std::nothrow) Logger<toFile> () );
    PointerProvider::fileLoggerProvider ( fileLoggerEngine );
#endif // !_NOT_DEBUGGING

    std::shared_ptr<Configurations> settings ( new (std::nothrow) Configurations () );
    PointerProvider::configurationProvider ( settings );

    if ((anException) && (settings))
      running = true;
    else
    {
      // failure, shut down the game properly
      if (anException)
      {
        PointerProvider::exceptionProvider ( nullptr );
        anException.reset ();
      }

      PointerProvider::getException ()->set ( "appSone" );
      throw* PointerProvider::getException ();
    }

#ifndef _NOT_DEBUGGING
    if (fileLoggerEngine)
    {
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Exception, file logger and configuration providers are successfully initialized." );
    } else
    {
      // failure, shut down the game properly
      if (anException)
      {
        PointerProvider::exceptionProvider ( nullptr );
        anException.reset ();
      }
      if (settings)
      {
        PointerProvider::configurationProvider ( nullptr );
        settings.reset ();
      }

      PointerProvider::getException ()->set ( "appDebug" );
      throw* PointerProvider::getException ();
    }
#endif // !_NOT_DEBUGGING


    GameWrapper game ( hInstance );

    if (game.isReady ())
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"The game is successfully initialized." );
#endif // !_NOT_DEBUGGING

      gameState = L"initialized";

    } else
    {
      game.shutdown (); // failure, try to save all the logs, while shutting down properly.

      PointerProvider::getException ()->set ( "appStwo" );
      throw* PointerProvider::getException ();
    }

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), L"mainThread",
                                              L"Entering the game loop..." );
#endif // !_NOT_DEBUGGING

    bool failure { false };

    if (!game.run ())
    {
      failure = true;

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), L"mainThread",
                                                L"A game functionality failed!" );
#endif // !_NOT_DEBUGGING

    }

    game.shutdown (); // failure or success, try to save all the logs, while shutting down properly.

    if (anException)
    {
      PointerProvider::exceptionProvider ( nullptr );
      anException.reset ();
    }

#ifndef _NOT_DEBUGGING
    if (fileLoggerEngine)
    {
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"The logging engine is going to successfully shut down..." );
      std::this_thread::sleep_for ( std::chrono::milliseconds { 100 } );

      PointerProvider::fileLoggerProvider ( nullptr );
      fileLoggerEngine.reset ();
    }
#endif // !_NOT_DEBUGGING

    if (settings)
    {
      PointerProvider::configurationProvider ( nullptr );
      settings.reset ();
    }

    gameState = L"uninitialized";

    if (failure)
      return EXIT_FAILURE;
    else
      return EXIT_SUCCESS;

  }
  catch (const std::exception& ex)
  {

    if (ex.what () == "appSone")
      MessageBoxA ( NULL, "The Game could not be started...", "Error", MB_OK | MB_ICONERROR );
    else
      if (ex.what () == "appDebug")
        MessageBoxA ( NULL, "The debug service failed to start.", "Error", MB_OK | MB_ICONERROR );
      else
        if (ex.what () == "appStwo")
        {

#ifndef _NOT_DEBUGGING
          PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                    L"The game initialization failed!" );
#endif // !_NOT_DEBUGGING

          MessageBoxA ( NULL, "The Game functionality failed to start...", "Critical-Error", MB_OK | MB_ICONERROR );
        } else
        {
          MessageBoxA ( NULL, ex.what (), "Error", MB_OK | MB_ICONERROR );
          if (debugger)
          {

#ifndef _NOT_DEBUGGING
            PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                      Converter::strConverter ( ex.what () ) );
            std::this_thread::sleep_for ( std::chrono::milliseconds { 100 } );
#endif // !_NOT_DEBUGGING

          }
        }
        return EXIT_FAILURE;
  }
}
