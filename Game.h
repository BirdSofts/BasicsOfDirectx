// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,24.07.2019</created>
/// <changed>ʆϒʅ,16.08.2019</changed>
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
  Microsoft::WRL::ComPtr<ID3D10Buffer> vertexBuffer; // vertex buffer
  bool initialized; // true if initialization was successful
  bool allocated; // true if resources allocation was successful
public:
  GameWrapper ( HINSTANCE& ); // game initialization
  void allocateResources (); // resources creation
  const bool& isReady ( void ); // get the initialized state
  void render ( void ); // render the scene
  const bool run ( void ); // game engine loop
  //void update ( void ); // updating the game world
  void shutdown ( void ); // destruction preparations
};


#endif // !GAME_H
