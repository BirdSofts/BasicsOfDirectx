// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.09.2019</created>
/// <changed>ʆϒʅ,02.09.2019</changed>
// ********************************************************************************

#ifndef LIGHT_H
#define LIGHT_H


#include "Direct3D.h"
#include "Camera.h"


// lightening wrapper
//-- diffuse lighting of type directional lighting:
//- similar to the sun light illuminated from a great distance away
//- using its direction the amount of light is determinable
//- unlike ambient lighting, no effect of surfaces it doesn't directly touch
//- requirements: direction and a normal polygon vector implemented in both pixel and vertex shader
class Light
{
private:
  Direct3D* d3d; // pointer to Direct3D application



  bool initialized; // true if initialization was successful
public:
  Light ( Direct3D* );
  const bool& isInitialized ( void ); // get the initialized state
};


#endif // !LIGHT_H
