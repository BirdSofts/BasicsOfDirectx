// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,24.07.2019</changed>
// ********************************************************************************

#ifndef DIRECTX_H
#define DIRECTX_H


class DirectX3dCore
{
private:
  HINSTANCE hInstance;
  bool initialized;
public:

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

  DirectX3dCore ( HINSTANCE& );
  ~DirectX3dCore ();
  const bool& initialState ();
};


#endif // !DIRECTX_H
