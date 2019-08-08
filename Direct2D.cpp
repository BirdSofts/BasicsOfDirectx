// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,04.08.2019</created>
/// <changed>ʆϒʅ,08.08.2019</changed>
// ********************************************************************************

#include "Direct2D.h"
#include "Shared.h"


Direct2D::Direct2D ( TheCore* coreObj ) : core ( coreObj ),
initialized ( false )
{
  try
  {
    HRESULT hResult;

    // creation of DirectWrite factory
    // properties options: --shared (reuse of cached font data, thus better performance), --isolated
    hResult = DWriteCreateFactory ( DWRITE_FACTORY_TYPE_SHARED, __uuidof( IDWriteFactory ), &writeFactory );
    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"DirectWrite factory is successfully created." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "crDWf" );
      throw* PointerProvider::getException ();
    }

    // creation of Direct2D factory
    D2D1_FACTORY_OPTIONS options;
#ifndef _NOT_DEBUGGING
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION; // debug (error and warning messages, diagnostic informations)
#else
    options.debugLevel = D2D1_DEBUG_LEVEL_NONE; // shipping build and release
#endif // !_NOT_DEBUGGING
    // multi threaded: safe access to the factory from multiple threads
    hResult = D2D1CreateFactory ( D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof( ID2D1Factory1 ), &options, &factory );
    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Direct2D factory is successfully created." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "crD2Df" );
      throw* PointerProvider::getException ();
    }

    // retrieving the DXGI device (Direct2D device and its context needs)
    // note that the DXGI associated to the Direct3D device must be obtained.
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    // returns a pointer to the interface being requested.
    hResult = core->d3d->device.Get ()->QueryInterface ( __uuidof( IDXGIDevice ), &dxgiDevice );
    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The DXGI device is successfully retrieved." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "getDd" );
      throw* PointerProvider::getException ();
    }

    // creation of the Direct2D device
    // threading mode will be inherited from the DXGI device.
    hResult = factory->CreateDevice ( dxgiDevice.Get (), &device );
    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Direct2D device is successfully created." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "crD2Dd" );
      throw* PointerProvider::getException ();
    }

    dxgiDevice->Release (); // not needed any more

    // creation of the Direct2D device context
    // the option: distribute all the rendering process across multiple threads.
    hResult = device->CreateDeviceContext ( D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &deviceContext );
    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Direct2D device context was successfully created." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "crD2Ddc" );
      throw* PointerProvider::getException ();
    }

    createBitmapRenderTarget ();
    initializeTextFormats ();

    initialized = true;

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Direct2D is successfully initialized." );
#endif // !_NOT_DEBUGGING

  }
  catch ( const std::exception& ex )
  {
    if ( ex.what () == "crDWf" )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"The creation of DirectWrite factory failed!" );
#endif // !_NOT_DEBUGGING

    } else
      if ( ex.what () == "crD2Df" )
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"The creation of Direct2D factory failed!" );
#endif // !_NOT_DEBUGGING

      } else
        if ( ex.what () == "getDd" )
        {

#ifndef _NOT_DEBUGGING
          PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Retrieving the DXGI device failed!" );
#endif // !_NOT_DEBUGGING

        } else
          if ( ex.what () == "crD2Dd" )
          {

#ifndef _NOT_DEBUGGING
            PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Creation of Direct2D device failed!" );
#endif // !_NOT_DEBUGGING

          } else
            if ( ex.what () == "crD2Ddc" )
            {

#ifndef _NOT_DEBUGGING
              PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Creation of Direct2D device context failed!" );
#endif // !_NOT_DEBUGGING

            } else
            {

#ifndef _NOT_DEBUGGING
              PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

            }
  }
};


void Direct2D::createBitmapRenderTarget ( void )
{
  try
  {
    HRESULT hResult;

    // setting the render target of Direct2D to the same back buffer as Direct3D
    // --bitmap (the actual rendering surface) properties (Direct2D needs)
    D2D1_BITMAP_PROPERTIES1 bitMap;
    bitMap.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM; // the same as Direct3D back buffer
    bitMap.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    bitMap.dpiX = 96.0f; // dots per inch of the bitmap
    bitMap.dpiY = 96.0f;
    bitMap.bitmapOptions =
      D2D1_BITMAP_OPTIONS_TARGET | // usable for the device context target
      D2D1_BITMAP_OPTIONS_CANNOT_DRAW; // not for use as an input
    bitMap.colorContext = nullptr; // a colour context interface

    // --retrieving the DXGI version of the Direct3D back buffer (Direct2D needs)
    Microsoft::WRL::ComPtr<IDXGISurface> dxgiBuffer;
    hResult = core->d3d->swapChain->GetBuffer ( 0, __uuidof( IDXGISurface ), &dxgiBuffer );
    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The back buffer needed for Direct2D is successfully retrieved." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "backB" );
      throw* PointerProvider::getException ();
    }

    Microsoft::WRL::ComPtr<ID2D1Bitmap1> targetBitmap; // new bitmap object to hold the desired properties
    // --the actual creation of the render target (retrieve the back buffer and set)
    hResult = deviceContext->CreateBitmapFromDxgiSurface ( dxgiBuffer.Get (), &bitMap, &targetBitmap );
    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The Direct2D bitmap is successfully created from the DXDI surface." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "crD2Db" );
      throw* PointerProvider::getException ();
    }

    dxgiBuffer->Release (); // not needed any more

    // --finally set the bitmap as render target (the same back buffer as Direct3D)
    deviceContext->SetTarget ( targetBitmap.Get () );

    //targetBitmap->Release (); // not needed any more

  }
  catch ( const std::exception& ex )
  {
    if ( ex.what () == "backB" )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Retrieving the back buffer needed for Direct2D failed!" );
#endif // !_NOT_DEBUGGING

    } else
      if ( ex.what () == "crD2Db" )
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Creation of Direct2D bitmap from the DXGI surface failed!" );
#endif // !_NOT_DEBUGGING

      } else
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

      }
  }
};


void Direct2D::initializeTextFormats ( void )
{
  try
  {
    HRESULT hResult;

    // creation of standard brushes
    hResult = deviceContext->CreateSolidColorBrush ( D2D1::ColorF ( D2D1::ColorF::Yellow ), &brushYellow );
    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The yellow brush is successfully created." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "crYeB" );
      throw* PointerProvider::getException ();
    }

    hResult = deviceContext->CreateSolidColorBrush ( D2D1::ColorF ( D2D1::ColorF::White ), &brushWhite );
    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The white brush is successfully created." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "crWhB" );
      throw* PointerProvider::getException ();
    }

    hResult = deviceContext->CreateSolidColorBrush ( D2D1::ColorF ( D2D1::ColorF::Black ), &brushBlack );
    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The black brush is successfully created." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "crBlB" );
      throw* PointerProvider::getException ();
    }

    // text format: nullptr: use system font collection
    hResult = writeFactory.Get ()->CreateTextFormat ( L"Lucida Console", nullptr, DWRITE_FONT_WEIGHT_EXTRA_LIGHT, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 13.0f, L"en-GB", &textFormatFPS );
    if ( FAILED ( hResult ) )
    {
      PointerProvider::getException ()->set ( "crFPStF" );
      throw* PointerProvider::getException ();
    }

    // text alignment
    hResult = textFormatFPS->SetTextAlignment ( DWRITE_TEXT_ALIGNMENT_LEADING );
    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The FPS text is successfully aligned." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "alFPSt" );
      throw* PointerProvider::getException ();
    }

    // paragraph alignment
    hResult = textFormatFPS->SetParagraphAlignment ( DWRITE_PARAGRAPH_ALIGNMENT_NEAR );
    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The FPS paragraph is successfully aligned." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "alFPSp" );
      throw* PointerProvider::getException ();
    }

  }
  catch ( const std::exception& ex )
  {
    if ( ex.what () == "crYeB" )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Creation of yellow brush failed!" );
#endif // !_NOT_DEBUGGING

    } else
      if ( ex.what () == "crWhB" )
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Creation of white brush failed!" );
#endif // !_NOT_DEBUGGING

      } else
        if ( ex.what () == "crBlB" )
        {

#ifndef _NOT_DEBUGGING
          PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Creation of black brush failed!" );
#endif // !_NOT_DEBUGGING

        } else
          if ( ex.what () == "crFPStF" )
          {

#ifndef _NOT_DEBUGGING
            PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Creation of FPS text format failed!" );
#endif // !_NOT_DEBUGGING

          } else
            if ( ex.what () == "alFPSt" )
            {

#ifndef _NOT_DEBUGGING
              PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Alignment of FPS text failed!" );
#endif // !_NOT_DEBUGGING

            } else
              if ( ex.what () == "alFPSp" )
              {

#ifndef _NOT_DEBUGGING
                PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Alignment of FPS paragraph failed!" );
#endif // !_NOT_DEBUGGING

              } else
              {

#ifndef _NOT_DEBUGGING
                PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

              }
  }
};


const bool& Direct2D::isInitialized ()
{
  return initialized;
};


void Direct2D::printFPS ( void )
{
  try
  {
    if ( core->showFPS && textLayoutFPS )
    {
      // drawing operations must be issued between a BeginDraw and EndDraw calls
      deviceContext->BeginDraw ();
      // drawing a fully formatted text
      deviceContext->DrawTextLayout ( D2D1::Point2F ( 2.0f, 5.0f ), textLayoutFPS.Get (), brushYellow.Get (), D2D1_DRAW_TEXT_OPTIONS_NONE );
      if ( FAILED ( deviceContext->EndDraw () ) )
      {
        PointerProvider::getException ()->set ( "drFPS" );
        throw* PointerProvider::getException ();
      }
    }
  }
  catch ( const std::exception& ex )
  {
    if ( ex.what () == "drFPS" )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Drawing the FPS information on screen failed!" );
#endif // !_NOT_DEBUGGING

    } else
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

    }
  }
};


void Direct2D::shutdown ( void )
{
  try
  {
    if ( core )
    {
      core = nullptr;
      delete core;
    }

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Direct2D is successfully destructed." );
#endif // !_NOT_DEBUGGING

  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};
