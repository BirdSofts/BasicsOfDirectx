// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,29.07.2019</changed>
// ********************************************************************************

#ifndef DIRECTX_H
#define DIRECTX_H


#include "Window.h"
#include "Timer.h"


class DirectX3dCore
{
  friend class Window;
private:
  HINSTANCE appInstance;
  Window* appWindow;
  HWND appHandle;
  bool initialized;
  bool paused;
  Timer* timer;
  int fps; // frames per second
  double frameRenderTime; // render time of a frame in milliseconds
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
  const bool& initialState ();
  const HINSTANCE& getInstance ();
  const HWND& getHandle ();
  const bool& pauseState ();
  Timer* getTimer ();
  void resize ( void );
  void frameStatistics (); // frame statistics calculations
  void shutdown ( void );
};


#endif // !DIRECTX_H
