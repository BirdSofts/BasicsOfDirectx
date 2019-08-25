// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,25.08.2019</created>
/// <changed>ʆϒʅ,26.08.2019</changed>
// ********************************************************************************

#ifndef CAMERA_H
#define CAMERA_H


#include "Core.h"


// camera wrapper
class Camera
{
  friend class Direct3D;
private:
  DirectX::XMFLOAT3 position; // camera position data
  DirectX::XMFLOAT3 rotation; // camera rotation data

  DirectX::XMMATRIX matrixView; // camera view matrix

  bool initialized; // true if initialization was successful
public:
  Camera ( void );
  const bool& isInitialized ( void ); // get the initialized state
  void setPosition ( float&, float&, float& ); // set camera position
  void forwardBackward ( float ); // set position forward/backward
  void render ( void ); // view matrix generation/update based on the camera position
  //const DirectX::XMMATRIX& getView ( void ); // get view matrix
};


#endif // !CAMERA_H
