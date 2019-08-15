// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,15.08.2019</changed>
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
    if (timer->isInitialized ())
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Timer is successfully initialized." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "inT" );
      throw* PointerProvider::getException ();
    }

    // application window instantiation
    appWindow = new (std::nothrow) Window ( this );
    if (appWindow->isInitialized ())
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Window is successfully initialized." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "inW" );
      throw* PointerProvider::getException ();
    }
    appHandle = appWindow->getHandle (); // handle to the instantiated window

    // Direct3D 10 instantiation
    d3d = new (std::nothrow) Direct3D ( this );
    if (d3d->isInitialized ())
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Direct3D is successfully initialized." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "inD3D" );
      throw* PointerProvider::getException ();
    }

    // Direct2D 10 instantiation
    d2d = new (std::nothrow) Direct2D ( this );
    if (d2d->isInitialized ())
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Direct2D is successfully initialized." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "inD2D" );
      throw* PointerProvider::getException ();
    }

    initialized = true;

#ifndef _NOT_DEBUGGING
    debug = true;
#endif // !_NOT_DEBUGGING

  }
  catch (const std::exception& ex)
  {

    if (ex.what () == "inT")
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Timer initialization failed!" );
#endif // !_NOT_DEBUGGING

    } else
      if (ex.what () == "inW")
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Window initialization failed!" );
#endif // !_NOT_DEBUGGING

      } else
        if (ex.what () == "inD3D")
        {

#ifndef _NOT_DEBUGGING
          PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                    L"Direct3D initialization failed!" );
#endif // !_NOT_DEBUGGING

        } else
          if (ex.what () == "inD2D")
          {

#ifndef _NOT_DEBUGGING
            PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                      L"Direct2D initialization failed!" );
#endif // !_NOT_DEBUGGING

          } else
          {

#ifndef _NOT_DEBUGGING
            PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                      Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

          }
  }
};


void TheCore::resizeResources ( void )
{
  try
  {

    if (initialized)
    {
      unsigned long refCountsCounts { 0 };
      HRESULT hResult;

      // free game resources
      if (game->vertexBuffer)
      {
        auto refCountsCounts = game->vertexBuffer.Reset ();
        //if (!refCountsCounts)
      }

      // free Direct2D resources
      if (d2d && !refCountsCounts)
      {
        refCountsCounts = d2d->textLayoutLogs.Reset ();
        if (!refCountsCounts)
          refCountsCounts = d2d->textLayoutFPS.Reset ();
        if (!refCountsCounts)
          refCountsCounts = d2d->textFormatLogs.Reset ();
        if (!refCountsCounts)
          refCountsCounts = d2d->textFormatFPS.Reset ();
        if (!refCountsCounts)
          refCountsCounts = d2d->brushBlack.Reset ();
        if (!refCountsCounts)
          refCountsCounts = d2d->brushWhite.Reset ();
        if (!refCountsCounts)
          refCountsCounts = d2d->brushYellow.Reset ();
        if (!refCountsCounts)
          refCountsCounts = d2d->dcBitmap.Reset ();
        if (!refCountsCounts)
          refCountsCounts = d2d->deviceCon.Reset ();
        if (!refCountsCounts)
        {
          HDC deviceConHandle; // HDC handle to the current device context
          // true: discard the Direct3D contents in the GDI device context
          hResult = d2d->dcBuffer->GetDC ( false, &deviceConHandle );
          // parameter: a rectangle representing the surface's dirty part to be released
          // note null: the whole surface is dirty
          // note am empty rectangle: if nothing has been changed
          hResult = d2d->dcBuffer->ReleaseDC ( nullptr );
        }
        //if (SUCCEEDED ( hResult ))
        refCountsCounts = d2d->dcBuffer.Reset ();
        if (refCountsCounts)
        {
          refCountsCounts = 0; // HACK Todo more research

#ifndef _NOT_DEBUGGING
          PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), L"mainThread",
                                                    L"Problem while releasing one or more resources!" );
#endif // !_NOT_DEBUGGING

        }
      }

      // free Direct3D resources
      if (d3d->dsView && d3d->rtView && !refCountsCounts)
      {
        refCountsCounts = d3d->inputLayout.Reset ();
        if (!refCountsCounts)
          refCountsCounts = d3d->pixelShader.Reset ();
        if (!refCountsCounts)
          refCountsCounts = d3d->vertexShader.Reset ();
        d3d->device->OMSetRenderTargets ( 0, nullptr, nullptr );
        if (!refCountsCounts)
          refCountsCounts = d3d->dsBuffer.Reset ();
        if (!refCountsCounts)
          refCountsCounts = d3d->dsView.Reset ();
        if (!refCountsCounts)
          refCountsCounts = d3d->rtView.Reset ();
        //if (!refCountsCounts)
        refCountsCounts = d3d->rtBuffer.Reset ();
        d3d->device->ClearState ();

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                  L"Deallocation of resources is successfully complete." );
#endif // !_NOT_DEBUGGING

      }

      // reallocation procedures
      if (!refCountsCounts)
      {
        d3d->allocateResources ();
        d2d->allocateResources ();
        d2d->initializeTextFormats ();
        game->allocateResources ();
        appWindow->isResized () = false;
        resized = true;

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                  L"Reallocation of resources is successfully complete." );
#endif // !_NOT_DEBUGGING

      } else
      {
        PointerProvider::getException ()->set ( "deR" );
        throw* PointerProvider::getException ();
      }
    }

  }
  catch (const std::exception& ex)
  {
    if (ex.what () == "deR")
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Deallocation of resources failed!" );
#endif // !_NOT_DEBUGGING

    } else
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                Converter::strConverter ( ex.what () ) );
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

#ifndef _NOT_DEBUGGING
      if (debug && !paused)
      {
        //// results to window caption
        //std::wstring caption = L"The Game ^,^ - FPS: " + std::to_wstring ( fps ) +
        //  L" - mSPF: " + std::to_wstring ( mspf );
        //SetWindowTextW ( appHandle, caption.c_str () );

        // results to client window area
        // FPS information text layouts
        std::wostringstream outFPS;
        outFPS.precision ( 6 );
        outFPS << "^_^ - FPS: " << fps << L" - mSPF: " << mspf << std::endl;

        // before rendering a text to a bitmap: the creation of the text layout
        hResult = d2d->writeFac->CreateTextLayout ( outFPS.str ().c_str (), ( UINT32) outFPS.str ().size (),
                                                    d2d->textFormatFPS.Get (), ( float) appWindow->clientWidth,
                                                    ( float) appWindow->clientHeight, &d2d->textLayoutFPS );
        if (FAILED ( hResult ))
        {
          PointerProvider::getException ()->set ( "crFPStL" );
          throw* PointerProvider::getException ();
        }

        std::wstring out { L"Last event: " };
        out += PointerProvider::getFileLogger ()->getLogRawStr ();
        hResult = d2d->writeFac->CreateTextLayout ( out.c_str (), ( UINT32) ( UINT32) out.size (),
                                                    d2d->textFormatLogs.Get (), ( float) appWindow->clientWidth,
                                                    ( float) appWindow->clientHeight, &d2d->textLayoutLogs );
        if (FAILED ( hResult ))
        {
          PointerProvider::getException ()->set ( "crLogsTL" );
          throw* PointerProvider::getException ();
        }
      }
#endif // !_NOT_DEBUGGING

      // reset
      frameCounter = 0;
      elapsed += 1.0;
    }
  }
  catch (const std::exception& ex)
  {
    if (ex.what () == "crFPStL")
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"The Creation of text layout for FPS information failed!" );
#endif // !_NOT_DEBUGGING

    } else
      if (ex.what () == "crLogsTL")
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"The Creation of text layout for Logs failed!" );
#endif // !_NOT_DEBUGGING

      } else
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

      }
  }
};


void TheCore::shutdown ( void )
{
  try
  {
    unsigned long refCounts { 0 };
    //HRESULT hResult;

    initialized = false;
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

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Application main window class is successfully destructed." );
#endif // !_NOT_DEBUGGING

      delete appWindow;
    }
    if (d2d)
    {
      d2d->initialized = false;
      refCounts = d2d->textLayoutLogs.Reset ();
      refCounts = d2d->textLayoutFPS.Reset ();
      refCounts = d2d->textFormatLogs.Reset ();
      refCounts = d2d->textFormatFPS.Reset ();
      refCounts = d2d->brushBlack.Reset ();
      refCounts = d2d->brushWhite.Reset ();
      refCounts = d2d->brushYellow.Reset ();
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

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Direct2D is successfully destructed." );
#endif // !_NOT_DEBUGGING

      delete d2d;
    }
    if (d3d)
    {
      d3d->initialized = false;
      d3d->device->ClearState ();
      refCounts = d3d->inputLayout.Reset ();
      refCounts = d3d->pixelShader.Reset ();
      refCounts = d3d->vertexShader.Reset ();
      d3d->device->OMSetRenderTargets ( 0, nullptr, nullptr );
      refCounts = d3d->dsBuffer.Reset ();
      refCounts = d3d->dsView.Reset ();
      refCounts = d3d->rtBuffer.Reset ();
      refCounts = d3d->rtView.Reset ();
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

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Direct3D is successfully destructed." );
#endif // !_NOT_DEBUGGING

      //d3d = nullptr;
      // debug exception while freeing the dynamic memory.
      delete d3d;
    }
    if (timer)
      delete timer;
    if (appInstance)
      appInstance = NULL;

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"The Application Core is successfully shut down." );
#endif // !_NOT_DEBUGGING

  }
  catch (const std::exception& ex)
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};
