// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,24.07.2019</created>
/// <changed>ʆϒʅ,17.08.2019</changed>
// ********************************************************************************

#include "Core.h"

#ifndef GAME_H
#define GAME_H


class GameWrapper
{
  friend class TheCore;
  friend class Direct3D;
  friend class Direct2D;
private:
  TheCore* core; // pointer to the framework core
  Microsoft::WRL::ComPtr<ID3D10Buffer> vertexBufferTriangle; // triangle vertex buffer
  Microsoft::WRL::ComPtr<ID3D10Buffer> vertexBufferLine; // line vertex buffer
  D3D10_MAPPED_TEXTURE2D mappedLine; // mapped structure to update the resource (D3D11_MAPPED_SUBRESOURCE)

  bool initialized; // true if initialization was successful
  bool allocated; // true if resources allocation was successful
public:
  GameWrapper ( HINSTANCE& ); // game initialization
  void allocateResources (); // resources creation
  const bool& isReady ( void ); // get the initialized state
  void render ( void ); // render the scene
  void update ( void ); // updating the game world
  const bool run ( void ); // game engine loop
  void shutdown ( void ); // destruction preparations
};


#endif // !GAME_H
