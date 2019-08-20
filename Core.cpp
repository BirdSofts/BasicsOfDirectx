// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,20.08.2019</changed>
// ********************************************************************************

#include "Core.h"
#include "Shared.h"


TheCore::TheCore ( HINSTANCE& hInstance, GameWrapper* gameObj ) :
  appInstance ( hInstance ), timer ( nullptr ),
  fps ( 0 ), mspf ( 0 ), appHandle ( NULL ), appWindow ( nullptr ),
  initialized ( false ), paused ( false ), debug ( false ),
  d3d ( nullptr ), d2d ( nullptr ), game ( gameObj ), resized ( false )
{
  try
  {

    // timer instantiation
    timer = new (std::nothrow) Timer ();
    if (!timer->isInitialized ())
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Timer initialization failed!" );
      return;
    }

    // application window instantiation
    appWindow = new (std::nothrow) Window ( this );
    if (!appWindow->isInitialized ())
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Window initialization failed!" );
      return;
    }
    appHandle = appWindow->getHandle (); // handle to the instantiated window

    // Direct3D 10 instantiation
    d3d = new (std::nothrow) Direct3D ( this );
    if (!d3d->isInitialized ())
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Direct3D initialization failed!" );
      return;
    }

    // Direct2D 10 instantiation
    d2d = new (std::nothrow) Direct2D ( this );
    if (!d2d->isInitialized ())
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Direct2D initialization failed!" );
      return;
    }

    initialized = true;
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"The framework is successfully initialized." );

    debug = true; // Todo must be switched from within the application

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
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


const int& TheCore::getFPS ( void )
{
  return fps;
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

    HRESULT hResult;

    // a static local variable retains its state between the calls:
    static int frameCounter; // frame counter (a frame is a full cycle of the game loop)
    static double elapsed; // the elapsed time since the last call
    frameCounter++;

    if ((timer->getTotal () - elapsed) >= 1e0)
    {

      // frame calculations:
      fps = frameCounter; // the number of counted frames in one second
      mspf = 1e3 / fps; // average taken time by a frame in milliseconds

      if (!paused && debug && d2d)
      {
        d2d->textLayoutsDebug = false;
        //// results to window caption
        //std::wstring caption = L"The Game ^,^ - FPS: " + std::to_wstring ( fps ) +
        //  L" - mSPF: " + std::to_wstring ( mspf );
        //SetWindowTextW ( appHandle, caption.c_str () );

        // results to client window area
        // FPS information text layouts
        std::wostringstream outFPS;
        outFPS.precision ( 6 );
        outFPS << "Resolution: " << d3d->displayMode.Width << " x " << d3d->displayMode.Height;
        outFPS << " - Display mode #" << d3d->displayModeIndex + 1 << " of " << d3d->displayModesCount << " @ ";
        outFPS << d3d->displayMode.RefreshRate.Numerator / d3d->displayMode.RefreshRate.Denominator << " Hz" << std::endl;
        outFPS << "^_^ - FPS: " << fps << L" - mSPF: " << mspf << std::endl;

        // before rendering a text to a bitmap: the creation of the text layout
        hResult = d2d->writeFac->CreateTextLayout ( outFPS.str ().c_str (), ( UINT32) outFPS.str ().size (),
                                                    d2d->textFormatFPS.Get (), ( float) appWindow->clientWidth,
                                                    ( float) appWindow->clientHeight, &d2d->textLayoutFPS );
        if (FAILED ( hResult ))
        {
          PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                    L"The Creation of text layout for FPS information failed!" );
          return;
        }

        std::wstring out { L"Last event: " };
        out += PointerProvider::getFileLogger ()->getLogRawStr ();
        hResult = d2d->writeFac->CreateTextLayout ( out.c_str (), ( UINT32) ( UINT32) out.size (),
                                                    d2d->textFormatLogs.Get (), ( float) appWindow->clientWidth,
                                                    ( float) appWindow->clientHeight, &d2d->textLayoutLogs );
        if (FAILED ( hResult ))
        {
          PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                    L"The Creation of text layout for Logs failed!" );
          return;
        }
        d2d->textLayoutsDebug = true;

      }

      // reset
      frameCounter = 0;
      elapsed += 1.0;
    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void TheCore::setResolution ( const bool& prm )
{
  try
  {

    if (prm)
    {
      if (!d3d->fullscreen)
      {
        d3d->displayModeIndex = d3d->displayModesCount - 1;
        d3d->displayMode = d3d->displayModes [d3d->displayModeIndex];
        d3d->fullscreen = true;
      }
    } else
    {
      if (d3d->fullscreen)
      {
        d3d->displayModeIndex = 0;
        d3d->displayMode = d3d->displayModes [d3d->displayModeIndex];
        d3d->fullscreen = false;
      }
    }

    resizeResources ( true );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void TheCore::resizeResources ( const bool& displayMode )
{
  try
  {

    if (initialized)
    {
      unsigned long refCounts { 0 };
      //HRESULT hResult;

      // free game resources
      if (game->vertexBufferTriangle)
      {
        refCounts = game->vertexBufferLine.Reset ();
        refCounts = game->vertexBufferTriangle.Reset ();
      }

      // free Direct2D resources
      if (d2d && !refCounts)
      {
        refCounts = d2d->dcBitmap.Reset ();
        refCounts = d2d->deviceCon.Reset ();
        refCounts = d2d->dcBuffer.Reset ();
        if (refCounts)
        {
          refCounts = 0; // HACK Todo more research
          PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), L"mainThread",
                                                    L"Problem while releasing one or more resources!" );
        }
      }

      // free Direct3D resources
      if (d3d->dsView && d3d->rtView && !refCounts)
      {
        refCounts = d3d->inputLayout.Reset ();
        refCounts = d3d->pixelShader.Reset ();
        refCounts = d3d->vertexShader.Reset ();
        d3d->device->OMSetRenderTargets ( 0, nullptr, nullptr );
        refCounts = d3d->dsBuffer.Reset ();
        refCounts = d3d->dsSurface.Reset ();
        refCounts = d3d->dsView.Reset ();
        refCounts = d3d->rtView.Reset ();
        if (refCounts)
        {
          refCounts = 0; // HACK Todo more research
          PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), L"mainThread",
                                                    L"Problem while releasing one or more resources!" );
        }
        refCounts = d3d->rtBuffer.Reset ();
        d3d->device->ClearState ();

        PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                  L"Deallocation of resources is successfully complete." );
      }

      // reallocation procedures
      if (!refCounts)
      {
        if (displayMode)
        {
          d3d->displayModeSetter ();
          std::this_thread::sleep_for ( std::chrono::milliseconds ( 2000 ) );
        }
        d3d->allocateResources ();
        if (d2d)
        {
          d2d->allocateResources ();
          d2d->initializeTextFormats ();
        }
        game->allocateResources ();
        appWindow->isResized () = false;
        resized = true;

        PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                  L"Reallocation of resources is successfully complete." );

      } else
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Deallocation of resources failed!" );
      }
    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void TheCore::shutdown ( void )
{
  try
  {

    unsigned long refCounts { 0 };
    //HRESULT hResult;

    initialized = false;

    // application main window destruction
    if (appWindow)
    {
      appWindow->initialized = false;
      if (appWindow->core)
      {
        appWindow->core = nullptr;
        delete appWindow->core;
      }
      if (appWindow->handle)
        appWindow->handle = NULL;
      if (appWindow->appInstance)
        appWindow->appInstance = NULL;
      delete appWindow;
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Application main window class is successfully destructed." );
    }

    // Direct2D application destruction
    if (d2d)
    {
      d2d->initialized = false;
      refCounts = d2d->dcBitmap.Reset ();
      refCounts = d2d->deviceCon.Reset ();
      //refCounts = d2d->dcBuffer.Reset ();
      refCounts = d2d->device.Reset ();
      refCounts = d2d->factory.Reset ();
      //refCounts = d2d->writeFac.Reset ();
      if (d2d->core)
      {
        d2d->core = nullptr;
        delete d2d->core;
      }
      delete d2d;
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Direct2D is successfully destructed." );
    }

    // Direct3D application destruction
    if (d3d)
    {
      d3d->initialized = false;
      d3d->device->ClearState ();
      refCounts = d3d->inputLayout.Reset ();
      refCounts = d3d->pixelShader.Reset ();
      refCounts = d3d->vertexShader.Reset ();
      d3d->device->OMSetRenderTargets ( 0, nullptr, nullptr );
      refCounts = d3d->dsSurface.Reset ();
      refCounts = d3d->dsBuffer.Reset ();
      refCounts = d3d->dsView.Reset ();
      refCounts = d3d->rtBuffer.Reset ();
      refCounts = d3d->rtView.Reset ();
      refCounts = d3d->output.Reset ();
      refCounts = d3d->swapChain.Reset ();

#ifndef _NOT_DEBUGGING
      refCounts = d3d->debug.Reset ();
#endif // !_NOT_DEBUGGING

      refCounts = d3d->dxgiDevice.Reset ();
      refCounts = d3d->dxgiAdapter.Reset ();
      refCounts = d3d->dxgiFactory.Reset ();
      //refCounts = d3d->device.Reset ();
      if (d3d->core)
      {
        d3d->core = nullptr;
        delete d3d->core;
      }
      //xx d3d = nullptr;
      //xx debug exception while freeing the dynamic memory.
      delete d3d;
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Direct3D is successfully destructed." );
    }

    // timer application destruction
    if (timer)
      delete timer;
    if (appInstance)
      appInstance = NULL;

    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"The Application Core is successfully shut down." );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};
