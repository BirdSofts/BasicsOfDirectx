// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,24.07.2019</created>
/// <changed>ʆϒʅ,26.08.2019</changed>
// ********************************************************************************

#include "Core.h"

#ifndef GAME_H
#define GAME_H


// vertex data
struct Vertex
{
  DirectX::XMFLOAT3 position;
  DirectX::XMFLOAT4 color;
};


class GameWrapper
{
  friend class TheCore;
  friend class Direct3D;
  friend class Direct2D;
private:
  TheCore* core; // pointer to the framework core
  Microsoft::WRL::ComPtr<ID3D10Buffer> vertexBuffer [2]; // triangle, line vertex buffer
  Microsoft::WRL::ComPtr<ID3D10Buffer> indexBuffer [2]; // triangle, line index buffer
  unsigned int vertexCountTriangle; // triangles' vertices count
  unsigned int vertexCountLine; // line's vertices count

  D3D10_MAPPED_TEXTURE2D mappedLine; // mapped structure to update the resource (D3D11_MAPPED_SUBRESOURCE)

  bool initialized; // true if initialization was successful
  bool allocated; // true if resources allocation was successful
public:
  GameWrapper ( HINSTANCE& ); // game initialization
  void allocateResources ( void ); // resources creation
  const bool& isReady ( void ); // get the initialized state
  void render ( void ); // render the scene
  void update ( void ); // updating the game world
  const bool run ( void ); // game engine loop
  void shutdown ( void ); // destruction preparations
};


#endif // !GAME_H
