// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,04.08.2019</changed>
// ********************************************************************************

#include "LearningDirectX.h"
#include "Core.h"
#include "Shared.h"


TheCore::TheCore ( HINSTANCE& hInstance ) :
  appInstance ( hInstance ), timer ( nullptr ),
  fps ( 0 ), frameRenderTime ( 0 ), appWindow ( nullptr ),
  initialized ( false ), paused ( false ), d3d ( nullptr )
{
  try
  {
    // timer instantiation
    timer = new ( std::nothrow ) Timer ();
    if ( timer->isInitialized () )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Timer is initialized." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "inT" );
      throw* PointerProvider::getException ();
    }

    // application window instantiation
    appWindow = new ( std::nothrow ) Window ( this );
    if ( appWindow->isInitialized () )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Window is initialized." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "inW" );
      throw* PointerProvider::getException ();
    }
    // handle of the instantiated window
    appHandle = appWindow->getHandle ();

    // Direct3D 10 instantiation
    d3d = new ( std::nothrow ) Direct3D ( this );
    if ( d3d->isInitialized () )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Direct3D is successfully initialized." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "inD" );
      throw* PointerProvider::getException ();
    }

    initialized = true;

  }
  catch ( const std::exception& ex )
  {

    if ( ex.what () == "inT" )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Timer initialization failed." );
#endif // !_NOT_DEBUGGING

    } else
      if ( ex.what () == "inW" )
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Window initialization failed." );
#endif // !_NOT_DEBUGGING

      } else
        if ( ex.what () == "inD" )
        {

#ifndef _NOT_DEBUGGING
          PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Direct3D initialization failed." );
#endif // !_NOT_DEBUGGING

        } else
        {

#ifndef _NOT_DEBUGGING
          PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

        }
  }
};


const bool& TheCore::isInitialized ( void )
{
  return initialized;
};


const HINSTANCE& TheCore::getInstance ( void )
{
  return appInstance;
};


const HWND& TheCore::getHandle ( void )
{
  return appHandle;
};


const bool& TheCore::isPaused ( void )
{
  return paused;
};


Timer* TheCore::getTimer ( void )
{
  return timer;
};


void TheCore::frameStatistics ( void )
{
  try
  {
    // a static local variable retains its state between the calls:
    static int frameCounter; // frame counter (a frame is a full cycle of the game loop)
    static double elapsed; // the elapsed time since the last call
    frameCounter++;

    if ( ( timer->getTotal () - elapsed ) >= 1 )
    {
      // frame calculations:
      fps = frameCounter; // the number of counted frames in one second
      frameRenderTime = 1e3 / fps; // average taken time by a frame in milliseconds

      // results to caption
      std::wstring caption = L"The Game ^,^ --- fps: " + std::to_wstring ( fps ) +
        L" --- frame render time in milliseconds: " + std::to_wstring ( frameRenderTime );
      SetWindowTextW ( appHandle, caption.c_str () );

      // reset
      frameCounter = 0;
      elapsed += 1.0;
    }
  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};


void TheCore::testDirect3D ( float arg [] )
{
  d3d->dev->ClearRenderTargetView ( d3d->renderTargetView.Get (), arg );
  d3d->present ();
};


void TheCore::shutdown ( void )
{
  try
  {
    appWindow->shutdown ();
    if ( appWindow )
      delete appWindow;
    d3d->shutdown ();
    if ( d3d )
      delete d3d;
    if ( timer )
      delete timer;
    if ( appInstance )
      appInstance = NULL;
    initialized = 0;

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The Application Core is successfully shut down." );
#endif // !_NOT_DEBUGGING

  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};
