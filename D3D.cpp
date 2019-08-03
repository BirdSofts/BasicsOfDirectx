// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,03.08.2019</changed>
// ********************************************************************************

#include "LearningDirectX.h"
#include "D3D.h"
#include "Shared.h"


Direct3D::Direct3D ( TheCore* coreObject ) : theCore ( coreObject ), initialized ( false )
// reserve 8 bits for blue, green, red and transparency each in unsigned normalized integer
, colourFormat ( DXGI_FORMAT_B8G8R8A8_UNORM )
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
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Direct3D device is succssessfuly created." );
#endif // !_NOT_DEBUGGING

      initialized = true;
    } else
    {
      PointerProvider::getException ()->set ( "crD" );
      throw* PointerProvider::getException ();
    }

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
      // ---- physical GPU identification
      hResult = dxgiDevice->GetAdapter ( dxgiAdapter.GetAddressOf () );
      if ( SUCCEEDED ( hResult ) )
        // ---- retrieving the factory
        hResult = dxgiAdapter->GetParent ( __uuidof( IDXGIFactory ), &dxgiFactory );
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
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The presentaiotn of the scene failed." );
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


void Direct3D::resize ( void )
{
  try
  {
    // BufferCount and SwapChainFlags: 0 do not change the current
    // the next two parameters: adjust to current client size of the target window
    // next parameter: set to DXGI_FORMAT_UNKNOWN to preserve the current
    if ( SUCCEEDED ( swapChain->ResizeBuffers ( 0, 0, 0, colourFormat, 0 ) ) )
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The graphic sizes are now adjusted." );
#endif // !_NOT_DEBUGGING

    } else
    {
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The graphic sizes could not be adjusted." );
#endif // !_NOT_DEBUGGING

      }
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
