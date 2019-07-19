// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,20.07.2019</changed>
// ********************************************************************************

#ifndef WINDOW_H
#define WINDOW_H


// functions declaration
bool initializeWindow ( HINSTANCE hInstance, int nShowCmd, int w, int h, bool windowed ); // window initialization
int loop (); // main part (game engine)
LRESULT CALLBACK winProcedure ( HWND handle, UINT msg, WPARAM wPrm, LPARAM lPrm ); // window callback procedure


#endif // !WINDOW_H
