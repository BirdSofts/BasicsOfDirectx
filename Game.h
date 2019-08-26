// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,24.07.2019</created>
/// <changed>ʆϒʅ,27.08.2019</changed>
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


class Game
{
  friend class TheCore;
  friend class Direct3D;
  friend class Direct2D;
private:
  TheCore* core; // pointer to the framework core

  // 3D models buffer containers, drawn by invoked shaders that are compiled into vertex/pixel shaders
  Microsoft::WRL::ComPtr<ID3D10Buffer> vertexBuffer [2]; // triangle, line vertex buffer
  // Note index buffers purposes: record the location of each vertex introduced in vertex buffer,
  // achieving much hider speed, and helps to cache the vertices data in faster locations of video memory.
  Microsoft::WRL::ComPtr<ID3D10Buffer> indexBuffer [2]; // triangle, line index buffer
  unsigned int vertexCountTriangle; // triangles' vertices count
  unsigned int vertexCountLine; // line's vertices count

  D3D10_MAPPED_TEXTURE2D mappedLine; // mapped structure to update the resource (D3D11_MAPPED_SUBRESOURCE)

  bool initialized; // true if initialization was successful
  bool allocated; // true if resources allocation was successful
public:
  Game ( HINSTANCE& ); // game initialization
  void allocateResources ( void ); // resources creation
  const bool& isReady ( void ); // get the initialized state
  const bool run ( void ); // game engine loop
  void render ( void ); // render the scene
  void update ( void ); // updating the game world
  void shutdown ( void ); // destruction preparations
};


#endif // !GAME_H
