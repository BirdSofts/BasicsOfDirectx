// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,24.07.2019</created>
/// <changed>ʆϒʅ,01.09.2019</changed>
// ********************************************************************************

#include "Core.h"
#include "Texture.h"
#include "2Dmodels.h"


#ifndef GAME_H
#define GAME_H


class Game
{
  friend class TheCore;
  friend class Direct3D;
  friend class Direct2D;
private:
  TheCore* core; // pointer to the framework core

  Triangles* trianglesObj; // three triangles

  Line* lineObj; // a line (clockwise turn, dynamic rewrite)

  Texture<TargaHeader>* texture; // texture resource
  TexturedTriangles* texturedTrianglesObj; // two textured triangles

  bool initialized; // true if initialization was successful
  bool allocated; // true if resources allocation was successful
public:
  Game ( HINSTANCE& ); // game initialization
  void allocateResources ( void ); // resources creation
  //void validate ( void ); // validate the allocation of game resources
  const bool& isReady ( void ); // get the initialized state
  const bool run ( void ); // game engine loop
  void render ( void ); // render the scene
  void update ( void ); // updating the game world
  void shutdown ( void ); // destruction preparations
};


#endif // !GAME_H
