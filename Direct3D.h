// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,02.08.2019</created>
/// <changed>ʆϒʅ,10.08.2019</changed>
// ********************************************************************************

#ifndef DIRECT3D_H
#define DIRECT3D_H


#include <wrl/client.h> // Windows and COM wrappers (calls to DirectX)
#include <d3d10_1.h> // Windows standard DirectX3D APIs
#pragma comment (lib, "d3d10_1.lib") // linkage to the 'd3d10' library


#include "Core.h"
#include "Direct2D.h"


class Direct3D
{
  friend class TheCore;
  friend class Direct2D;
private:
  TheCore* core; // pointer to the application core

  // note that this application put the power of Direct3d 10 into practice.
  // to easily handle the life cycle of COM objects, they can be defined using smart pointers known as COM pointers,
  // note that each COM interface must be created in its own way.
  // the core of Direct3D is two COM objects the device and the device context:

  // -- the device object: virtual representation of the video adapter
  // purpose: access to GPU memory and creation of Direct3D COM objects.
  Microsoft::WRL::ComPtr<ID3D10Device1> device; // Direct3D device
  Microsoft::WRL::ComPtr<ID3D10Debug> debug; // live report is available from Direct3D 11

  // -- the device context: the structure defining a set of graphic objects
  // and their associated attributes, additionally graphic modes that affect output,
  // therefore it can be seen as the control panel of the GPU,
  // through which the transformation of three-dimensional to the final two-dimensional,
  // and the process of rendering that image to screen is controlled.
  //Microsoft::WRL::ComPtr<ID3D11DeviceContext> devConD11; // Direct3D device context

  // The COM interface representing the swap chain:
  // purpose: swapping the back buffers (double or triple) and drawing to the display surface
  Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain; // the swap chain
  Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice; // swap chain creation needs
  Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter; // swap chain creation needs
  Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory; // swap chain creation needs

  DXGI_FORMAT colourFormat; // colour format

  Microsoft::WRL::ComPtr<ID3D10RenderTargetView> rtView; // render target view
  Microsoft::WRL::ComPtr<ID3D10Texture2D> rtBuffer; // render target view buffer

  Microsoft::WRL::ComPtr<ID3D10DepthStencilView> dsView; // depth-stencil view
  Microsoft::WRL::ComPtr<ID3D10Texture2D> dsBuffer; // depth-stencil view buffer

  bool initialized; // true if the initialization was successful
  bool created; // true if the creation was successful
  bool resized; // true if the resizing was successful
public:
  Direct3D ( TheCore* ); // device creation plus swapping the chain
  const bool& isInitialized (); // get the initialized state
  void createResources ( void ); // create the device resources
  void resize ( void ); // resize the resources
  void clearBuffers ( void ); // clear depth-stencil buffers
  void present ( void ); // swapping: present the buffer chain by flipping the buffers
  void shutdown ( void ); // destruction preparations
};


#endif // !DIRECT3D_H
