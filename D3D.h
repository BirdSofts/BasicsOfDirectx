// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,02.08.2019</created>
/// <changed>ʆϒʅ,03.08.2019</changed>
// ********************************************************************************

#ifndef D3D_H
#define D3D_H


#include "Core.h"


class Direct3D
{
  friend class TheCore;
private:
  TheCore* theCore; // pointer to DirectX core application

  // the core of Direct3D 11 is two COM objects the device and the device context:
  // note that this application put the power of Direct3d 10 into practice.

  // -- the device object: virtual representation of the video adapter
  // purpose: access to GPU memory and creation of Direct3D COM objects.
  Microsoft::WRL::ComPtr<ID3D10Device> dev; // Direct3D device

  // -- the device context: the structure defining a set of graphic objects
  // and their associated attributes, additionally graphic modes that affect output.
  //Microsoft::WRL::ComPtr<ID3D11DeviceContext> devConD11; // Direct3D device context

  // The COM interface representing the swap chain:
  // purpose: swapping the back buffer (double or triple) and the display surface
  Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain; // the swap chain

  //Microsoft::WRL::ComPtr<ID3D10RenderTargetView*> renderTargetView;

  bool initialized; // true if the initialization was successful

  DXGI_FORMAT colourFormat; // colour format
public:
  Direct3D ( TheCore* ); // device creation plus swapping the chain
  const bool& isInitialized (); // get the initialized state
  void createResources ( void ); // create the device resources
  void present ( void ); // draw the scene
  void resize ( void ); // resize the resources
  void shutdown ( void ); // destruction preparations
};


#endif // !D3D_H
