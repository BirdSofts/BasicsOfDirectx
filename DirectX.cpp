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

  // 
  D3D10CreateDeviceAndSwapChain ( 0, D3D10_DRIVER_TYPE_HARDWARE, 0, 0, D3D10_SDK_VERSION, &swapChainD, &swapChain, &d3dDevice );

  ID3D10Texture2D* backBuffer;
  swapChain->GetBuffer ( 0, __uuidof( ID3D10Texture2D ), reinterpret_cast<void**>( &backBuffer ) );
  d3dDevice->CreateRenderTargetView ( backBuffer, 0, &renderTargetView );
  backBuffer->Release ();

  d3dDevice->OMSetRenderTargets ( 1, &renderTargetView, NULL );

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
