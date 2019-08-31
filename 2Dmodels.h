﻿// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,31.08.2019</created>
/// <changed>ʆϒʅ,01.09.2019</changed>
// ********************************************************************************

#ifndef _2D_MODELS_H
#define _2D_MODELS_H


#include <d3d10_1.h>
#include <DirectXMath.h>
#include <string>


struct Vertex
{
  DirectX::XMFLOAT3 position;
  DirectX::XMFLOAT4 color;
};


struct VertexT
{
  DirectX::XMFLOAT3 position;
  DirectX::XMFLOAT2 texture;
};


// 2D object model base class
template <class tType>
class O2Dmodel
{
protected:
  ID3D10Device1* device;

  D3D10_BUFFER_DESC vertexBufferDesc;
  // 2D/3D models buffer containers, drawn by invoked shaders that are compiled into vertex/pixel shaders
  ID3D10Buffer* vertexBuffer; // models' vertex buffer

  D3D10_SUBRESOURCE_DATA subResourceDate; // to interface with the object model vertices as resources

  D3D10_BUFFER_DESC indexBufferDesc;
  ID3D10Buffer* indexBuffer; // models' index buffer
  // Note index buffers purposes: record the location of each vertex introduced in vertex buffer,
  // achieving much hider speed, and helps to cache the vertices data in faster locations of video memory.

  std::wstring entryPoint;
  bool dynamic; // true: dynamic usage + write access for CPU
  bool allocate ( tType*, unsigned long*, unsigned long& ); // object model resources allocation
public:
  O2Dmodel ( ID3D10Device1*, std::wstring, bool );
  ID3D10Buffer** const getVertexBuffer ( void ); // vertex buffer
  ID3D10Buffer* const getIndexBuffer ( void ); // index buffer
  void release ( void ); // release the object model
};
void TextureClassLinker ( void ); // don't call this function: solution for linker error, when using templates.


class Triangles : public O2Dmodel<Vertex>
{
private:
  Vertex verticesData [9]; // 2D object model vertices data
  unsigned long verticesIndices [9]; // 2D object model vertices indices
public:
  unsigned long verticesCount; // 2D object model vertices count
  bool allocated; // true after successful resource allocation

  Triangles ( ID3D10Device1* );
};


class Line : public O2Dmodel<Vertex>
{
private:
  Vertex verticesData [2]; // 2D object model vertices data
  unsigned long verticesIndex [2]; // 2D object model vertices indices
public:
  unsigned long verticesCount; // 2D object model vertices count
  bool allocated; // true after successful resource allocation

  D3D10_MAPPED_TEXTURE2D mappedRes; // updating the resource
  // note DirectX 11: D3D11_MAPPED_SUBRESOURCE

  Line ( ID3D10Device1* );
  void update ( void );
};


class TexturedTriangles : public O2Dmodel<VertexT>
{
private:
  VertexT verticesData [6]; // 2D object model vertices data
  unsigned long verticesIndex [6]; // 2D object model vertices indices
public:
  unsigned long verticesCount; // 2D object model vertices count
  bool allocated; // true after successful resource allocation

  TexturedTriangles ( ID3D10Device1* );
};


#endif // !_2D_MODELS_H