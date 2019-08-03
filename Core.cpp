// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,03.08.2019</changed>
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
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Direct3D 10 is successfully initialized." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "inD" );
      throw* PointerProvider::getException ();
    }


    // the structure type to declare the swap chain:
    // -- BufferDesc: general properties of the back buffer
    DXGI_SWAP_CHAIN_DESC swapChainD;
    swapChainD.BufferDesc.Width = PointerProvider::getConfiguration ()->getSettings ().Width;
    swapChainD.BufferDesc.Height = PointerProvider::getConfiguration ()->getSettings ().Height;
    swapChainD.BufferDesc.RefreshRate.Numerator = 60;
    swapChainD.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainD.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainD.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    // no multi sampling
    // -- SampleDesc: the number of samples or the quality, needs to be done using antialiasing, but for now:
    swapChainD.SampleDesc.Count = 1;
    swapChainD.SampleDesc.Quality = 0;
    // -- BufferUsage: the value specifies, that the back buffer is the target to render to.
    swapChainD.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    // -- BufferCount: the number of back buffers used in swap chain. one back buffer for double buffering and two for triple buffering.
    swapChainD.BufferCount = 1;
    // handle to the window and the windowed or full-screen mode
    swapChainD.OutputWindow = appHandle;
    swapChainD.Windowed = true;
    // -- SwapEffect: the way to swap the back and front buffers.
    // the specified value provides the display driver the most efficient presentation technique for swap chain.
    swapChainD.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    // to introduce probable needed extra flags
    swapChainD.Flags = 0;

    // the actual device creation and swap chain by filling the structure
    D3D10CreateDeviceAndSwapChain (
      0, // the display adapter to be represented by created device (0 or NULL uses the primary display driver)
      // the driver type, accepting two usable options (HARDWARE and REFERENCE)
      // -- hardware is the actual graphic card
      // -- reference is slow, uses software implementation and is for testing purposes
      // note that the reference option is under consideration, if the graphics card can't use certain code.
      D3D10_DRIVER_TYPE_HARDWARE,
      // using hardware rasterizing, the software is set to zero,
      // therefore for being able to use software rasterizer, one needs to be already available.
      0,
      0, // optional device creation flags, and to be passed zero for release builds
      D3D10_SDK_VERSION, // SDKVersion and is always set to this value
      &swapChainD, // pointer to filled out swap chain structure type, representing the swap chain under process
      &swapChain, // returns the created swap chain (ppSwapChain)
      &d3dDevice ); // returns the created device (ppDevice)

    // a 2d texture for back buffer, onto which the scene will be rendered
    ID3D10Texture2D* backBuffer { nullptr };

    // passing the back buffer to swap chain
    // note that the back buffer is a COM object, therefore it is to be released at the end of the code.
    // __uuidof operator is a Microsoft language extension, using it the compiler extracts the GUID value from the header,
    // therefore no library export is necessary.
    swapChain->GetBuffer ( 0, __uuidof( ID3D10Texture2D ), reinterpret_cast<void**>( &backBuffer ) );

    // creating render target view:
    d3dDevice->CreateRenderTargetView (
      backBuffer, // the resource to be used as the render target
      // a pointer to a render target view DESC structure:
      // note that, in case the resource is created with a typed format, this parameter can be passed zero,
      // in all other cases the format type of the render target needs to be specified here
      0,
      &renderTargetView ); // a pointer to the created render target view object

    // back buffer and depth-stencil buffer are created and filled, therefore releasing the back buffer COM object
    backBuffer->Release ();

    // this method bind one or more render targets to the output merger stage of the pipeline
    d3dDevice->OMSetRenderTargets (
      // the number of the render targets passed to the pipeline
      // using advanced techniques, it is possible to simultaneously bind more than one to several render targets
      1,
      &renderTargetView, // a pointer to an array of render target view pointers to be bound to the pipeline
      NULL ); // a pointer to the depth-stencil view to be bound to the pipeline
    D3D10_VIEWPORT viewPort;
    viewPort.TopLeftX = 0;
    viewPort.TopLeftY = 0;
    viewPort.Width = PointerProvider::getConfiguration ()->getSettings ().Width;
    viewPort.Height = PointerProvider::getConfiguration ()->getSettings ().Height;
    viewPort.MinDepth = 0;
    viewPort.MaxDepth = 1;

    d3dDevice->RSSetViewports ( 1, &viewPort );

    float backColor [4] { 0,0,0,1 };
    d3dDevice->ClearRenderTargetView ( renderTargetView, backColor );

    swapChain->Present ( 0, 0 );

    initialized = true;

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Direct3D 10 is successfully initialized." );
#endif // !_NOT_DEBUGGING

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


const bool& TheCore::isInitialized ()
{
  return initialized;
};


const HINSTANCE& TheCore::getInstance ()
{
  return appInstance;
};


const HWND& TheCore::getHandle ()
{
  return appHandle;
};


const bool& TheCore::isPaused ()
{
  return paused;
};


Timer* TheCore::getTimer ()
{
  return timer;
};


void TheCore::frameStatistics ()
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
      frameRenderTime = fps * 0.001; // average taken time by a frame in milliseconds

      // results to caption
      std::wstring caption = L"The Game ^,^ --- fps: " + std::to_wstring ( fps ) +
        L" --- frame render time in milliseconds: " + std::to_wstring ( frameRenderTime );
      SetWindowTextW ( appHandle, caption.c_str () );

      // reset
      frameCounter = 0;
      elapsed += 1;
    }
  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};


void TheCore::shutdown ( void )
{
  try
  {
    appWindow->shutdown ();
    if ( timer )
      delete timer;
    if ( appWindow )
      delete appWindow;
    if ( appInstance )
      appInstance = NULL;
    initialized = 0;

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The DirectX3D is successfully uninitialized." );
#endif // !_NOT_DEBUGGING

  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};
