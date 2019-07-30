// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,31.07.2019</changed>
// ********************************************************************************

#include "LearningDirectX.h"
#include "DirectX.h"
#include "Shared.h"


DirectX3dCore::DirectX3dCore ( HINSTANCE& hInstance ) :
  appInstance ( hInstance ), appWindow ( nullptr ),
  initialized ( false ), paused ( false ), timer ( nullptr ),
  fps ( 0 ), frameRenderTime ( 0.0 )
{
  // timer instantiation
  timer = new Timer ();

  // application window instantiation
  appWindow = new Window ( this );

#ifndef _NOT_DEBUGGING
  if ( !appWindow->initialState () )
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Window initialization failed." );
  else
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Window is initialized." );
#endif // !_NOT_DEBUGGING

  // handle of the instantiated window
  appHandle = appWindow->getHandle ();

  // the structure type to declare the swap chain:
  // -- BufferDesc: general properties of the back buffer
  DXGI_SWAP_CHAIN_DESC swapChainD;
  swapChainD.BufferDesc.Width = PointerProvider::getConfiguration ()->get ().Width;
  swapChainD.BufferDesc.Height = PointerProvider::getConfiguration ()->get ().Height;
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
  viewPort.Width = PointerProvider::getConfiguration ()->get ().Width;
  viewPort.Height = PointerProvider::getConfiguration ()->get ().Height;
  viewPort.MinDepth = 0.0f;
  viewPort.MaxDepth = 1.0f;

  d3dDevice->RSSetViewports ( 1, &viewPort );

  float backColor [4] { 0.0f,0.0f,0.0f,1.0f };
  d3dDevice->ClearRenderTargetView ( renderTargetView, backColor );

  swapChain->Present ( 0, 0 );

  initialized = true;

#ifndef _NOT_DEBUGGING
  PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"DirectX3D is initialized." );
#endif // !_NOT_DEBUGGING

};


const bool& DirectX3dCore::initialState ()
{
  return initialized;
};


const HINSTANCE& DirectX3dCore::getInstance ()
{
  return appInstance;
};


const HWND& DirectX3dCore::getHandle ()
{
  return appHandle;
};


const bool& DirectX3dCore::pauseState ()
{
  return paused;
};


Timer* DirectX3dCore::getTimer ()
{
  return timer;
};


void DirectX3dCore::resize ( void )
{

#ifndef _NOT_DEBUGGING
  PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), L"mainThread", L"Since window is resized, the game graphics must be updated!" );
#endif // !_NOT_DEBUGGING

};


void DirectX3dCore::frameStatistics ()
{
  // a static local variable retains its state between the calls:
  static int frameCounter; // frame counter (a frame is a full cycle of the game loop)
  static double elapsed; // the elapsed time since the last call
  frameCounter++;

  if ( ( timer->getTotal () - elapsed ) >= 1.0 )
  {
    // frame calculations:
    fps = frameCounter; // the number of counted frames
    frameRenderTime = 1000.0 / ( double) fps; // average taken time by a frame

    // results to caption
    std::wstring caption = L"The Game ^,^ --- fps: " + std::to_wstring ( fps ) +
      L" --- frame render time in milliseconds: " + std::to_wstring ( frameRenderTime );
    SetWindowTextW ( appHandle, caption.c_str () );

    // reset
    frameCounter = 0;
    elapsed += 1.0;
  }
};


void DirectX3dCore::shutdown ( void )
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
  PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The DirectX3D is uninitialized." );
#endif // !_NOT_DEBUGGING
};
