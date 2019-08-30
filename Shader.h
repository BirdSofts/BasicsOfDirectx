// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.08.2019</created>
/// <changed>ʆϒʅ,29.08.2019</changed>
// ********************************************************************************

#ifndef SHADER_H
#define SHADER_H


#include "Core.h"
#include "Shared.h"


// shader buffer
struct ShaderBuffer
{
  byte* buffer;
  long long size;
  ShaderBuffer ( void );
  ~ShaderBuffer ( void );
  void Release ( void );
};


// shaders wrapper
class Shader
{
  friend class TheCore;
  friend class Direct3D;
  friend class Game;
private:
  Direct3D* d3d; // pointer to Direct3D application

  // indices: 0 color shader, 1 texture shader
  Microsoft::WRL::ComPtr<ID3D10VertexShader> vertexShader; // standard vertex shader
  Microsoft::WRL::ComPtr<ID3D10PixelShader> pixelShader; // standard pixel shader
  Microsoft::WRL::ComPtr<ID3D10InputLayout> inputLayout; // layout of the vertex data

  Microsoft::WRL::ComPtr<ID3D10Buffer> matrixBuffer; // constant matrix buffer (to interface with shader)

  Microsoft::WRL::ComPtr<ID3D10VertexShader> vertexShaderT; // texture vertex shader
  Microsoft::WRL::ComPtr<ID3D10PixelShader> pixelShaderT; // texture pixel shader
  Microsoft::WRL::ComPtr<ID3D10InputLayout> inputLayoutT; // layout of the texture vertex data
  Microsoft::WRL::ComPtr<ID3D10SamplerState> samplerState; // to interface with texture shader

  bool initialized; // true if initialization was successful
public:
  Shader ( Direct3D* );
  const bool& isInitialized ( void ); // get the initialized state
  void initializePipeline ( void ); // rendering pipeline (GPU) initialization
  void loadShader ( std::string&, ShaderBuffer* ); // read shader data (compiled .cso files)
};


#endif // !SHADER_H
