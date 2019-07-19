// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,17.07.2019</created>
/// <changed>ʆϒʅ,20.07.2019</changed>
// ********************************************************************************

#include "LearningDirectX.h"
#include "Window.h"
#include "DirectX.h"


// references:
// https://www.braynzarsoft.net/


float r { 0.0f };
float g { 0.0f };
float b { 0.0f };
int colourMod_r { 1 };
int colourMod_g { 1 };
int colourMod_b { 1 };


// main function (main window)
int WINAPI WinMain ( _In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR lpCmdLine,
                     _In_ int nShowCmd )
{
  if ( !initializeWindow ( hInstance, nShowCmd, Width, Height, true ) )
  {
    MessageBox ( 0, L"Window initialization failed.", L"Error", MB_OK | MB_ICONERROR );
    return 1;
  }
  if ( !InitializeD3dApp ( hInstance ) )
  {
    MessageBox ( 0, L"Direct3D initialization failed.", L"Error", MB_OK | MB_ICONERROR );
    return 1;
  }
  if ( !InitializeScene () )
  {
    MessageBox ( 0, L"Scene initialization failed.", L"Error", MB_OK | MB_ICONERROR );
    return 1;
  }
  MSG msg; // a new message structure
  ZeroMemory ( &msg, sizeof ( MSG ) ); // clear the structure to zero

  // main part (game engine)
  while ( true ) // continuous loop
  {
    // check and peek window messages
    if ( PeekMessage ( &msg, NULL, 0, 0, PM_REMOVE ) )
    {
      if ( msg.message == WM_QUIT ) // exit
        break;
      TranslateMessage ( &msg ); // translation of the massage
      DispatchMessage ( &msg ); // sending to default window procedure
    } else
    {

      r += colourMod_r * 0.00002f;
      g += colourMod_g * 0.00002f;
      b += colourMod_b * 0.00010f;

      if ( ( r >= 1.0f ) || ( r <= 0.0f ) )
        colourMod_r *= -1;
      if ( ( g >= 1.0f ) || ( g <= 0.0f ) )
        colourMod_g *= -1;
      if ( ( b >= 1.0f ) || ( b <= 0.0f ) )
        colourMod_b *= -1;

      float backColor [4] { r, g, b, 1.0f };

      d3dDevice->ClearRenderTargetView ( renderTargetView, backColor );

      swapChain->Present ( 0, 0 );

    }
  }
  return 0;
}
