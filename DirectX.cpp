// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,20.07.2019</changed>
// ********************************************************************************

#include "LearningDirectX.h"
#include "DirectX.h"


// Direct3D interface, in charge of controlling the hardware device,
// the structure type is basically the software controller to the graphic device
ID3D10Device* d3dDevice;

// swap chain interface, a structure type to hold the front and back buffer,
// which are 2d textures to draw the scenes on, while the front buffer already appears on the screen,
// the back buffer is waiting to appear on the screen.
// these two buffers are on the row first calculated as back buffer and then shown as front buffer,
// this way the flickering effect is avoided, with other words,
// the calculation and drawing happen and what is going to be seen is the completed pictures.
IDXGISwapChain* swapChain;

// render target view interface, a structure type that gets the resource view and sends it to the wished pipeline stage.
// for the Direct3D to be able to render onto back buffer, it needs to be bound to the output merger stage,
// therefore a render target view is created for the back buffer.
ID3D10RenderTargetView* renderTargetView;


// Direct3D initialization
bool InitializeD3dApp ( HINSTANCE hInstance )
{
  // the structure type to declare the swap chain:

  // -- BufferDesc: general properties of the back buffer
  DXGI_SWAP_CHAIN_DESC swapChainD;
  swapChainD.BufferDesc.Width = Width;
  swapChainD.BufferDesc.Height = Height;
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
  swapChainD.OutputWindow = handle;
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
  ID3D10Texture2D* backBuffer;

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
  viewPort.Width = Width;
  viewPort.Height = Height;
  viewPort.MinDepth = 0.0f;
  viewPort.MaxDepth = 1.0f;

  d3dDevice->RSSetViewports ( 1, &viewPort );

  float backColor [4] { 0.0f,0.0f,0.0f,1.0f };
  d3dDevice->ClearRenderTargetView ( renderTargetView, backColor );

  swapChain->Present ( 0, 0 );

  return true;
}

bool InitializeScene ()
{
  return true;
}

bool DrawTheScene ()
{
  // draw the scene here
  return true;
}
