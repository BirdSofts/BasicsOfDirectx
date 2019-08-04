// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,04.08.2019</changed>
// ********************************************************************************

#include "LearningDirectX.h"
#include "D3D.h"
#include "Shared.h"


Direct3D::Direct3D ( TheCore* coreObject ) : theCore ( coreObject ),
// reserve 8 bits for red, green, blue and transparency each in unsigned normalized integer
colourFormat ( DXGI_FORMAT_R8G8B8A8_UNORM ),
initialized ( false ), created ( false ), resized ( false )

{
  try
  {

    // flag: needed to get Direct2D interoperability with Direct3D resources
    unsigned int deviceFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT;

#ifndef _NOT_DEBUGGING
    deviceFlags |= D3D10_CREATE_DEVICE_DEBUG; // creation with debug layer
#endif // !_NOT_DEBUGGING

    // the actual device creation
    HRESULT hResult = D3D10CreateDevice ( nullptr, D3D10_DRIVER_TYPE_HARDWARE, NULL, deviceFlags, D3D10_SDK_VERSION, &dev );

    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Direct3D device is successfully created." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "crD" );
      throw* PointerProvider::getException ();
    }

    createResources ();

    if ( created && resized )
      initialized = true;

  }
  catch ( const std::exception& ex )
  {

    if ( ex.what () == "crD" )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"The creation of Direct3D device failed!" );
#endif // !_NOT_DEBUGGING

    } else
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

    }
  }
};


const bool& Direct3D::isInitialized ()
{
  return initialized;
};


void Direct3D::createResources ( void )
{
  try
  {

    HRESULT hResult;

    // swap chain creation:

    // -- filling a swap chain description structure
    DXGI_SWAP_CHAIN_DESC scd;
    scd.BufferDesc.Width = 0; // back buffer size, 0: automatic adjustment
    scd.BufferDesc.Height = 0; // the same
    scd.BufferDesc.RefreshRate.Numerator = 0; // 0: don't care and don't correct it
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferDesc.Format = colourFormat; // display format
    scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // scan-line drawing
    scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // image size adjustment to back buffer resolution
    // number of multi samplings per pixel and image quality (1 and 0: disable multi sampling (no anti-aliasing))
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // back buffer as render output target
    scd.BufferCount = 3; // including the front buffer (one front buffer and two back buffers)
    scd.OutputWindow = theCore->getHandle (); // handle to main window
    scd.Windowed = true; // recommendation: windowed creation and switch to full screen
    // flip (in windowed mode: blit) and discard the content of back buffer after presentation
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow switching the display mode

    // -- getting the DXGI factory:
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
    Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;
    // ---- retrieving the underlying DXGI device
    hResult = dev.As ( &dxgiDevice );
    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Retrieving the underlying DXGI device was successful." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "getF" );
      throw PointerProvider::getException ();
    }

    // ---- physical GPU identification
    hResult = dxgiDevice->GetAdapter ( dxgiAdapter.GetAddressOf () );
    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The Physical GPU identification is now complete." );
#endif // !_NOT_DEBUGGING
    } else
    {
      PointerProvider::getException ()->set ( "getF" );
      throw PointerProvider::getException ();
    }

    // ---- retrieving the factory
    hResult = dxgiAdapter->GetParent ( __uuidof( IDXGIFactory ), &dxgiFactory );
    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Retrieving the factory was successful." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "getF" );
      throw PointerProvider::getException ();
    }

    // -- now create the swap chain
    hResult = dxgiFactory->CreateSwapChain ( dev.Get (), &scd, swapChain.GetAddressOf () );
    if ( SUCCEEDED ( hResult ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Swap chain is successfully created!" );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "crS" );
      throw PointerProvider::getException ();
    }

    created = true;

    resize ();

  }
  catch ( const std::exception& ex )
  {
    if ( ex.what () == "getF" )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"The DGGI Adapter was unable to get the factory!" );
#endif // !_NOT_DEBUGGING

    } else
      if ( ex.what () == "crS" )
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Swap chain creation resulted to failure!" );
#endif // !_NOT_DEBUGGING

      } else
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

      }

  }
};


void Direct3D::resize ( void )
{
  try
  {
    dev->ClearState ();
    renderTargetView = nullptr;
    depthStencilView = nullptr;

    // resizing the swap chain
    // BufferCount and SwapChainFlags: 0 do not change the current
    // the next two parameters: adjust to current client size of the target window
    // next parameter: set to DXGI_FORMAT_UNKNOWN to preserve the current
    if ( SUCCEEDED ( swapChain->ResizeBuffers ( 0, 0, 0, colourFormat, 0 ) ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The buffers of swap chain are successfully resized." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "reS" );
      throw PointerProvider::getException ();
    }

    // the render target view recreation
    Microsoft::WRL::ComPtr<ID3D10Texture2D> backBuffer;
    // obtain a pointer to the current back buffer in the swap chain
    // the zero-th buffer is accessible, since already created using flip discarding effect.
    // second parameter: interface type (most cases 2D- texture)
    // the last parameter returns a pointer to the actual back buffer
    if ( FAILED ( swapChain->GetBuffer ( 0, __uuidof( ID3D10Texture2D ), reinterpret_cast<void**>( backBuffer.GetAddressOf () ) ) ) )
    {
      PointerProvider::getException ()->set ( "backB" );
      throw PointerProvider::getException ();
    }

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Swap chain back buffer is obtained." );
#endif // !_NOT_DEBUGGING

    // first parameter: the resource for which the render target is created for
    // second parameter describes data type of the specified resource (mipmap but 0 for now)
    // the last parameter returns a pointer to the created render target view
    if ( FAILED ( dev->CreateRenderTargetView ( backBuffer.Get (), NULL, &renderTargetView ) ) )
    {
      PointerProvider::getException ()->set ( "crR" );
      throw PointerProvider::getException ();
    }

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The swap chain is successfully recreated." );
#endif // !_NOT_DEBUGGING

    // depth and stencil buffer creation
    CD3D10_TEXTURE2D_DESC dsd;
    Microsoft::WRL::ComPtr<ID3D10Texture2D> dsBuffer;
    backBuffer->GetDesc ( &dsd ); // retrieves the description of the back buffer and fill! :)
    dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 bits for depth and 8 bits for stencil
    //dsd.SampleDesc // multi-sampling (anti-aliasing) match to settings of render target
    dsd.Usage = D3D10_USAGE_DEFAULT; // value: only GPU will be reading and writing to the resource
    dsd.BindFlags = D3D10_BIND_DEPTH_STENCIL; // how to bind to the different pipeline stages
    // texture creation:
    // the second parameter: pointer to initial data (zero for any data, since depth/stencil buffer)
    if ( FAILED ( dev->CreateTexture2D ( &dsd, NULL, dsBuffer.GetAddressOf () ) ) )
    {
      PointerProvider::getException ()->set ( "cr2Dt" );
      throw PointerProvider::getException ();
    }

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The creation of texture was successful." );
#endif // !_NOT_DEBUGGING

    // depth/stencil view creation
    // the second parameter: zero to access the mipmap level 0
    if ( FAILED ( dev->CreateDepthStencilView ( dsBuffer.Get (), NULL, depthStencilView.GetAddressOf () ) ) )
    {
      PointerProvider::getException ()->set ( "crD-Sb" );
      throw PointerProvider::getException ();
    }

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The creation of depth/stencil view was successful." );
#endif // !_NOT_DEBUGGING

    // depth and stencil buffer activation (for now one render target view)
    // second parameter: pointer to first element of a list of render target view pointers
    dev->OMSetRenderTargets ( 1, renderTargetView.GetAddressOf (), depthStencilView.Get () );

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Depth/stencil buffer is now activated." );
#endif // !_NOT_DEBUGGING

    // set the viewpoint to entire back buffer (what area should be rendered to)
    D3D10_VIEWPORT vp;
    vp.TopLeftX = 0; // first four integers: viewport rectangle (relative to client window rectangle)
    vp.TopLeftY = 0;
    vp.Width = dsd.Width;
    vp.Height = dsd.Height;
    vp.MinDepth = 0.0f; // minimum and maximum depth buffer values
    vp.MaxDepth = 1.0f;
    // setting the viewport
    // the second parameter is a pointer to an array of viewports
    dev->RSSetViewports ( 1, &vp );

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The view port is successfully set." );
#endif // !_NOT_DEBUGGING

    resized = true;
    //clearBuffers ();

  }
  catch ( const std::exception& ex )
  {
    if ( ex.what () == "reS" )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Direct3D could not resize the swap chain!" );
#endif // !_NOT_DEBUGGING

    } else
      if ( ex.what () == "backB" )
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Direct3D could not acquire the back buffer!" );
#endif // !_NOT_DEBUGGING

      } else
        if ( ex.what () == "crR" )
        {

#ifndef _NOT_DEBUGGING
          PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Direct3D could not create the render target view!" );
#endif // !_NOT_DEBUGGING

        } else
          if ( ex.what () == "cr2Dt" )
          {

#ifndef _NOT_DEBUGGING
            PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Direct3D could not create a 2D-texture!" );
#endif // !_NOT_DEBUGGING

          } else
            if ( ex.what () == "crD-Sb" )
            {

#ifndef _NOT_DEBUGGING
              PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Direct3D could not create the depth and stencil buffer!" );
#endif // !_NOT_DEBUGGING

            } else
            {

#ifndef _NOT_DEBUGGING
              PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

            }
  }
};


void Direct3D::clearBuffers ( void )
{
  try
  {
    dev->ClearState ();
    renderTargetView = nullptr;
    depthStencilView = nullptr;

    // make ready for the new scenes after each frame (clear all leftover artefacts)
    float black [] { 0.0f, 0.0f, 0.0f, 0.0f };
    // filling the entire back buffer with a single colour
    dev->ClearRenderTargetView ( renderTargetView.Get (), black );
    // second parameter: the type of data to clear (obviously set to clear both depth and stencil)
    // the values are used to override the entire depth/stencil buffer with
    dev->ClearDepthStencilView ( depthStencilView.Get (), D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0 );

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"All leftover artefacts are now cleared from buffers." );
#endif // !_NOT_DEBUGGING

  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};


void Direct3D::present ( void )
{
  try
  {
    // SyncInterval parameter: the way a frame is synchronized with VBLANK:
    // in flip mode: the n = 0 sets the DirectX to cancel the remaining time of previously rendered scene
    // and discard this frame if a newer frame is on the queue. (screen tearing might occur)
    // the n = 1 to 4 values: synchronize the presentation after n-th vertical blank.
    // the second parameter: not waiting for v-sync.
    HRESULT hResult { swapChain->Present ( 0, DXGI_PRESENT_DO_NOT_WAIT ) };

    if ( ( FAILED ( hResult ) ) &&
      ( hResult != DXGI_ERROR_WAS_STILL_DRAWING ) ) // occurs, if the calling thread is blocked
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), L"mainThread", L"The presentation of the scene failed." );
#endif // !_NOT_DEBUGGING

    }
  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};


void Direct3D::shutdown ( void )
{
  try
  {
    if ( theCore )
    {
      theCore = nullptr;
      delete theCore;
    }

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Direct3D is successfully destructed." );
#endif // !_NOT_DEBUGGING

  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};
