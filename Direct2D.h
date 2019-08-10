// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,04.08.2019</created>
/// <changed>ʆϒʅ,10.08.2019</changed>
// ********************************************************************************

#ifndef DIRECT2D_H
#define DIRECT2D_H


#include <wrl/client.h> // Windows and COM wrappers (calls to DirectX)
#include <d2d1_1.h> // DirectX includes
#pragma comment (lib, "d2d1.lib") // linkage to the 'd2d1' library
#include <dwrite_1.h> // DirectX includes
#pragma comment (lib, "dwrite.lib") // linkage to the 'dwrite' library


#include "Core.h"
#include "Direct3D.h"


class Direct2D
{
  friend class TheCore;
  friend class Direct3D;
private:
  TheCore* core; // pointer to the application core

  Microsoft::WRL::ComPtr<IDWriteFactory1> writeFac; // DirectWrite factory
  Microsoft::WRL::ComPtr<ID2D1Factory1> factory; // Direct2D factory
  Microsoft::WRL::ComPtr<ID2D1Device> device; // Direct2D device
  Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice; // DXGI device (Direct2D device needs)
  Microsoft::WRL::ComPtr<ID2D1DeviceContext> deviceCon; // Direct2D device context
  Microsoft::WRL::ComPtr<IDXGISurface> dcBuffer; // Direct2D device context buffer
  Microsoft::WRL::ComPtr<ID2D1Bitmap1> dcBitmap; // render target bitmap (desired properties)

  bool initialized; // true if the initialization was successful
public:
  Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushYellow; // brushes
  Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushWhite;
  Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushBlack;

  Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormatFPS; // text formats
  Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormatLogs;
  Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayoutFPS; // text layouts
  Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayoutLogs;

  Direct2D ( TheCore* ); // creation of the device
  void createResources ( void ); // creation of device context and bitmap render target
  void initializeTextFormats ( void ); // different formats initialization
  const bool& isInitialized (); // get the initialized state
  void debugPrint ( void ); // FPS to screen
  void shutdown ( void ); // destruction preparations
};


#endif // !DIRECT2D_H
