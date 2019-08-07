// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,02.08.2019</created>
/// <changed>ʆϒʅ,06.08.2019</changed>
// ********************************************************************************

#ifndef D3D_H
#define D3D_H


#include <wrl/client.h> // Windows and COM wrappers (calls to DirectX)
#include <d3d10_1.h> // Windows standard DirectX3D APIs
#pragma comment (lib, "d3d10_1.lib") // linkage to the 'd3d10' library


#include "Core.h"
#include "D2D.h"


class Direct3D
{
  friend class TheCore;
  friend class Direct2D;
private:
  TheCore* theCore; // pointer to DirectX core application

  // the core of Direct3D 11 is two COM objects the device and the device context:
  // note that this application put the power of Direct3d 10 into practice.

  // -- the device object: virtual representation of the video adapter
  // purpose: access to GPU memory and creation of Direct3D COM objects.
  Microsoft::WRL::ComPtr<ID3D10Device1> dev; // Direct3D device

  // -- the device context: the structure defining a set of graphic objects
  // and their associated attributes, additionally graphic modes that affect output.
  //Microsoft::WRL::ComPtr<ID3D11DeviceContext> devConD11; // Direct3D device context

  // The COM interface representing the swap chain:
  // purpose: swapping the back buffer (double or triple) and the display surface
  Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain; // the swap chain

  DXGI_FORMAT colourFormat; // colour format

  Microsoft::WRL::ComPtr<ID3D10RenderTargetView> renderTargetView; // rendering target

  Microsoft::WRL::ComPtr<ID3D10DepthStencilView> depthStencilView; // depth and stencil buffer

  bool initialized; // true if the initialization was successful
  bool created; // true if the creation was successful
  bool resized; // true if the resizing was successful
public:
  Direct3D ( TheCore* ); // device creation plus swapping the chain
  const bool& isInitialized (); // get the initialized state
  void createResources ( void ); // create the device resources
  void resize ( void ); // resize the resources
  void clearBuffers ( void ); // clear depth and stencil buffers
  void present ( void ); // present the chain by flipping the buffers
  void shutdown ( void ); // destruction preparations
};


#endif // !D3D_H
