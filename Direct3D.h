// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,02.08.2019</created>
/// <changed>ʆϒʅ,20.08.2019</changed>
// ********************************************************************************

#ifndef DIRECT3D_H
#define DIRECT3D_H


#include <wrl/client.h> // Windows and COM wrappers (calls to DirectX)
#include <d3d10_1.h> // Windows standard DirectX3D APIs
#pragma comment (lib, "d3d10_1.lib") // linkage to the 'd3d10' library


#include "Core.h"
#include "Direct2D.h"
#include "Shared.h"
#include "Game.h"


// vertex data
struct Vertex
{
  float x, y, z; // position
  float r, g, b; // colour
};


// shader buffer
struct ShaderBuffer
{
  byte* buffer;
  long long size;
  ShaderBuffer ();
  ~ShaderBuffer ();
};


// Direct3D wrapper
class Direct3D
{
  friend class TheCore;
  friend class Direct2D;
  friend class GameWrapper;
private:
  TheCore* core; // pointer to the framework core

  // note that this application put the power of Direct3d 10 into practice.
  // to easily handle the life cycle of COM objects, they can be defined using smart pointers known as COM pointers,
  // note that each COM interface must be created in its own way.
  // the core of Direct3D is two COM objects the device and the device context:

  // -- the device object: virtual representation of the video adapter
  // purpose: access to GPU memory and creation of Direct3D COM objects.
  Microsoft::WRL::ComPtr<ID3D10Device1> device; // Direct3D device
  Microsoft::WRL::ComPtr<IDXGIDevice1> dxgiDevice; // DXGI device
  Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter; // DXGI Adapter
  Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory; // DXGI Factory

#ifndef _NOT_DEBUGGING
  Microsoft::WRL::ComPtr<ID3D10Debug> debug; // Device Debug layer
#endif // !_NOT_DEBUGGING

  // -- the device context: the structure defining a set of graphic objects
  // and their associated attributes, additionally graphic modes that affect output,
  // therefore it can be seen as the control panel of the GPU,
  // through which the transformation of three-dimensional to the final two-dimensional,
  // and the process of rendering that image to screen is controlled.
  //Microsoft::WRL::ComPtr<ID3D11DeviceContext> devConD11; // Direct3D device context

  // The COM interface representing the swap chain:
  // purpose: swapping the back buffers (double or triple) and drawing to the display surface
  Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain; // the swap chain
  Microsoft::WRL::ComPtr<IDXGIOutput> output; // output adapter

  DXGI_FORMAT colourFormat; // colour format
  unsigned int displayModesCount; // the number of supported display modes
  DXGI_MODE_DESC* displayModes; // list of supported display modes
  unsigned int displayModeIndex; // the index of current display mode
  DXGI_MODE_DESC displayMode; // current display mode

  Microsoft::WRL::ComPtr<ID3D10RenderTargetView> rtView; // render target view
  Microsoft::WRL::ComPtr<ID3D10Texture2D> rtBuffer; // render target view buffer

  Microsoft::WRL::ComPtr<ID3D10DepthStencilView> dsView; // depth-stencil view
  Microsoft::WRL::ComPtr<ID3D10Texture2D> dsBuffer; // depth-stencil view buffer
  Microsoft::WRL::ComPtr<IDXGISurface1> dsSurface; // depth-stencil surface

  Microsoft::WRL::ComPtr<ID3D10VertexShader> vertexShader; // standard vertex shader
  Microsoft::WRL::ComPtr<ID3D10PixelShader> pixelShader; // standard pixel shader
  //static ShaderBuffer csoBuffer; // vertex/pixel shaders need
  Microsoft::WRL::ComPtr<ID3D10InputLayout> inputLayout;

  bool initialized; // true if initialization was successful
  bool allocated; // true if resources allocation was successful
  bool fullscreen; // application configuration
public:
  Direct3D ( TheCore* ); // creation of the device and resources
  const bool& isInitialized (); // get the initialized state
  const ID3D10Device1& getDevice ( void ); // get the pointer to application Direct3D
  const bool& isFullscreen (); // get the display mode state
  void displayModeSetter ( void ); // Direct3D display mode change/adjust
  void allocateResources ( void ); // Direct3D resources resize/creation
  void clearBuffers ( void ); // clear depth-stencil buffers
  void loadShader ( std::string&, ShaderBuffer* ); // read shader data (compiled .cso files)
  void initializePipeline ( void ); // rendering (GPU) pipeline initialization
  void present ( void ); // swapping: present the buffer chain by flipping the buffers
};


#endif // !DIRECT3D_H
