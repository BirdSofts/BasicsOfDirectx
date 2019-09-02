// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.08.2019</created>
/// <changed>ʆϒʅ,03.09.2019</changed>
// ********************************************************************************

#ifndef SHADER_H
#define SHADER_H


#include <d3d10_1.h>
#include <d3dcompiler.h> // standard DirectX3D compiler APIs (shader compiler)
#pragma comment (lib, "d3dcompiler.lib") // linkage to the 'd3dcompiler' library
#include <string>


// shader buffer
class Shader
{
  //friend class TheCore;
  //friend class Direct3D;
  //friend class Game;
private:
  struct Buffer // shader buffer
  {
    byte* buffer;
    long long size;
    Buffer ( void );
    ~Buffer ( void );
    void release ( void );
  };
protected:
  ID3D10Device1* device; // pointer to Direct3D device

  ID3D10VertexShader* vertexShader; // standard vertex shader
  ID3D10PixelShader* pixelShader; // standard pixel shader
  ID3D10InputLayout* inputLayout; // standard input layout
  ID3D10SamplerState* samplerState; // standard sampler state (textured shaders)

  std::wstring entryPoint;
public:
  Shader ( ID3D10Device1*, std::wstring );
  void loadShader ( std::string&, Buffer* ); // read shader data (compiled .cso files)
  bool initialize ( D3D10_INPUT_ELEMENT_DESC*,
                    D3D10_SAMPLER_DESC*, bool, bool ); // rendering pipeline (GPU initialization)
  ID3D10VertexShader* const getVertexShader ( void );
  ID3D10PixelShader* const getPixelShader ( void );
  ID3D10InputLayout* const getInputLayout ( void );
  ID3D10SamplerState** const getSamplerState ( void );
  void release ( void ); // release the shaders resources
};


class ShaderColour : public Shader
{
private:
  D3D10_INPUT_ELEMENT_DESC polygonLayoutDesc [2]; // input layout description
  unsigned int elementsCount;

  bool initialized; // true if initialization was successful
public:
  ShaderColour ( ID3D10Device1* );
  const bool& isInitialized ( void ); // get the initialized state
};


class ShaderTexture : public Shader
{
private:
  D3D10_INPUT_ELEMENT_DESC polygonLayoutDesc [2]; // input layout description
  unsigned int elementsCount;
  D3D10_SAMPLER_DESC samplerDesc; // tecture sampler state description

  bool initialized; // true if initialization was successful
public:
  ShaderTexture ( ID3D10Device1* );
  const bool& isInitialized ( void ); // get the initialized state
};


class ShaderLight : public Shader
{
private:
  //D3D10_INPUT_ELEMENT_DESC polygonLayoutDesc [3]; // input layout description
  //unsigned int elementsCount;

  bool initialized; // true if initialization was successful
public:
  ShaderLight ( ID3D10Device1* );
  const bool& isInitialized ( void ); // get the initialized state
};


#endif // !SHADER_H
