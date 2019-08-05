// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,04.08.2019</created>
/// <changed>ʆϒʅ,05.08.2019</changed>
// ********************************************************************************

#ifndef D2D_H
#define D2D_H


#include <wrl/client.h> // Windows and COM wrappers (calls to DirectX)
#include <d2d1_3.h> // DirectX includes
#pragma comment (lib, "d2d1.lib") // linkage to the 'd2d1' library
#include <dwrite_3.h> // DirectX includes
#pragma comment (lib, "dwrite.lib") // linkage to the 'dwrite' library


#include "Core.h"
#include "D3D.h"


class Direct2D
{
  friend class TheCore;
  friend class Direct3D;
private:
  TheCore* theCore; // pointer to DirectX core application

  //Microsoft::WRL::ComPtr<idwrite> dev; // Direct3D device
  //Microsoft::WRL::ComPtr<ID3D10Device> dev; // Direct3D device
  //Microsoft::WRL::ComPtr<ID3D10Device> dev; // Direct3D device

  bool initialized; // true if the initialization was successful
public:
  Direct2D ( TheCore* ); // device creation plus swapping the chain
  const bool& isInitialized (); // get the initialized state
  void shutdown ( void ); // destruction preparations
};


#endif // !D2D_H
