// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,08.08.2019</changed>
// ********************************************************************************

#include "Core.h"
#include "Shared.h"


TheCore::TheCore ( HINSTANCE& hInstance ) :
  appInstance ( hInstance ), timer ( nullptr ),
  fps ( 0 ), mspf ( 0 ), appHandle ( NULL ), appWindow ( nullptr ),
  initialized ( false ), paused ( false ), showFPS ( false ),
  d3d ( nullptr ), d2d ( nullptr )
{
  try
  {
    // timer instantiation
    timer = new ( std::nothrow ) Timer ();
    if ( timer->isInitialized () )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Timer is successfully initialized." );
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
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Window is successfully initialized." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "inW" );
      throw* PointerProvider::getException ();
    }
    appHandle = appWindow->getHandle (); // handle to the instantiated window

    // Direct3D 10 instantiation
    d3d = new ( std::nothrow ) Direct3D ( this );
    if ( d3d->isInitialized () )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Direct3D is successfully initialized." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "inD3D" );
      throw* PointerProvider::getException ();
    }

    // Direct2D 10 instantiation
    d2d = new ( std::nothrow ) Direct2D ( this );
    if ( d2d->isInitialized () )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Direct2D is successfully initialized." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "inD2D" );
      throw* PointerProvider::getException ();
    }

    initialized = true;

#ifndef _NOT_DEBUGGING
    showFPS = true;
#endif // !_NOT_DEBUGGING

  }
  catch ( const std::exception& ex )
  {

    if ( ex.what () == "inT" )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Timer initialization failed!" );
#endif // !_NOT_DEBUGGING

    } else
      if ( ex.what () == "inW" )
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Window initialization failed!" );
#endif // !_NOT_DEBUGGING

      } else
        if ( ex.what () == "inD3D" )
        {

#ifndef _NOT_DEBUGGING
          PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Direct3D initialization failed!" );
#endif // !_NOT_DEBUGGING

        } else
          if ( ex.what () == "inD2D" )
          {

#ifndef _NOT_DEBUGGING
            PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Direct2D initialization failed!" );
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


Direct3D* TheCore::getd3d ( void )
{
  return d3d;
};


Direct2D* TheCore::getd2d ( void )
{
  return d2d;
};


void TheCore::frameStatistics ( void )
{
  try
  {
    // a static local variable retains its state between the calls:
    static int frameCounter; // frame counter (a frame is a full cycle of the game loop)
    static double elapsed; // the elapsed time since the last call
    frameCounter++;

    if ( ( timer->getTotal () - elapsed ) >= 1e0 )
    {
      // frame calculations:
      fps = frameCounter; // the number of counted frames in one second
      mspf = 1e3 / fps; // average taken time by a frame in milliseconds

      // results to window caption
      //std::wstring caption = L"The Game ^,^ --- FPS: " + std::to_wstring ( fps ) +
      //  L" --- mSPF: " + std::to_wstring ( mspf );
      //SetWindowTextW ( appHandle, caption.c_str () );

      // results to client window area
      if ( showFPS )
      {
        // FPS information text layouts
        std::wostringstream outFPS;
        outFPS.precision ( 6 );
        outFPS << " ^,^ --- FPS: " << fps << L" --- mSPF: " << mspf << std::endl;

        // before rendering a text to a bitmap: the creation of the text layout
        HRESULT hResult = d2d->writeFactory->CreateTextLayout ( outFPS.str ().c_str (), ( UINT32) outFPS.str ().size (), d2d->textFormatFPS.Get (), ( float) appWindow->clientWidth, ( float) appWindow->clientHeight, &d2d->textLayoutFPS );
        if ( FAILED ( hResult ) )
        {
          PointerProvider::getException ()->set ( "crFPStL" );
          throw* PointerProvider::getException ();
        }
      }

      // reset
      frameCounter = 0;
      elapsed += 1.0;
    }
  }
  catch ( const std::exception& ex )
  {
    if ( ex.what () == "crFPStL" )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"The Creation of text layout for FPS information failed!" );
#endif // !_NOT_DEBUGGING

    } else
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

    }
  }
};


void TheCore::testDirect3D ( float arg [] )
{
  d3d->dev->ClearRenderTargetView ( d3d->renderTargetView.Get (), arg );
  d3d->dev->ClearDepthStencilView ( d3d->depthStencilView.Get (), D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0 );

  //d3d->present ();
};


void TheCore::shutdown ( void )
{
  try
  {
    appWindow->shutdown ();
    if ( appWindow )
      delete appWindow;
    d2d->shutdown ();
    d3d->shutdown ();
    if ( d2d )
      delete d2d;
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
