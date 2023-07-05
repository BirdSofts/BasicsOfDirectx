
// ===========================================================================
/// <summary>
/// Game.h
/// DirectXIntroduction
/// created by Mehrdad Soleimanimajd on 24.07.2019
/// </summary>
/// <created>ʆϒʅ, 24.07.2019</created>
/// <changed>ʆϒʅ, 04.07.2023</changed>
// ===========================================================================

#include "Core.h"
#include "Universe.h"
#include "Shader.h"
#include "Texture.h"
#include "Polygons.h"


#ifndef GAME_H
#define GAME_H


class Game
{
    friend class TheCore;
    friend class Direct3D;
    friend class Direct2D;
private:
    TheCore* core; // pointer to the framework core

    Universe* universe; // world, camera, lights...

    ShaderColour* shaderColour;
    ShaderTexture* shaderTexture;
    Texture<TargaHeader>* texture; // texture resource
    ShaderDiffuseLight* shaderDiffuseLight;

    Triangles* _2Dtriangles; // three triangles
    Line* _2Dline; // a line (clockwise turn, dynamic rewrite)
    TexturedTriangles* _2DtexturedTriangles; // two textured triangles
    LightedTriangle* _2DlightedTriangle; // one triangle illuminated by diffuse light

    Cube* _3Dcube;

    bool initialized; // true if initialization was successful
    bool allocated; // true if resources allocation was successful
public:
    Game (HINSTANCE&); // game initialization
    void allocateResources (void); // resources creation
    //void validate ( void ); // validate the allocation of game resources
    const bool& isReady (void); // get the initialized state
    const bool run (void); // game engine loop
    void render (void); // render the scene
    void update (void); // updating the game world
    Universe* getUniverse (void); // get the pointer to game universe
    void shutdown (void); // destruction preparations
};


#endif // !GAME_H
