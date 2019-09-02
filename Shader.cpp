// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.08.2019</created>
/// <changed>ʆϒʅ,03.09.2019</changed>
// ********************************************************************************


#include "Shader.h"
#include "Shared.h"


Shader::Shader ( ID3D10Device1* dev, std::wstring entry ) :
  device ( dev ), entryPoint ( entry )
{
  // shaders, introduced in several different types,
  // are used to actually render vertices or pixels to the screen in the render process,
  // controlled by the graphics rendering pipeline, which is programmed by these shaders,
  // and each of them is a small program that controls one step of the pipeline.
  // the process in nature takes vertices as input and results in fully rendered images,
  // in which each type of shader is run many times based on its different nature.
  // note that shaders are written in HLSL (High Level Shader Language),
  // Visual Studio is able to create HLSL programs, and after compiling,
  // the HLSL file is compiled into CSO (Compiled Shader Objects), usable by the running program.
  // note that it is essential to write high efficient vertex shaders: http://www.rastertek.com/dx10s2tut04.html

  vertexShader = nullptr;
  pixelShader = nullptr;
  inputLayout = nullptr;
  samplerState = nullptr;
};


Shader::Buffer::Buffer ( void )
{
  buffer = nullptr;
  size = 0;
};


Shader::Buffer::~Buffer ( void )
{
  if (buffer)
    release ();
};


void Shader::Buffer::release ( void )
{
  if (buffer)
    delete buffer;
  buffer = nullptr;
};


void Shader::loadShader ( std::string& fileName, Buffer* csoBuffer )
{
  try
  {

    // load shaders from .cso compiled files

    std::string path { "" };

#ifndef _NOT_DEBUGGING
    path = { ".//x64//Debug//" };
#else
    path = { ".//x64//Release//" };
#endif // !_NOT_DEBUGGING

    path += fileName;
    std::ifstream csoFile ( path, std::ios::binary | std::ios::ate );
    if (csoFile.is_open ())
    {
      csoBuffer->size = csoFile.tellg (); // shader object size
      csoBuffer->buffer = new (std::nothrow) byte [csoBuffer->size];
      if (csoBuffer->buffer)
      {
        csoFile.seekg ( 0, std::ios::beg );
        csoFile.read ( reinterpret_cast<char*>(csoBuffer->buffer), csoBuffer->size );
      } else
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Shader buffer allocation for compiled file failed!" + entryPoint );
      }
      csoFile.close ();
    } else
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Loading shader form compiled file failed!" + entryPoint );
    }
  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) + entryPoint );
  }
};


bool Shader::initialize ( D3D10_INPUT_ELEMENT_DESC* polygonLayout,
                          D3D10_SAMPLER_DESC* sampler,
                          bool compile, bool textured = false )
{
  try
  {

    HRESULT hR;

    if (!compile)
    {
      // load and encapsulate .cso shaders into usable shader objects

      std::string fileName { "" };

      fileName = "vertex.cso"; // compiled by VisualStudio
      Buffer vertexBuffer; // vertex shader buffer
      loadShader ( fileName, &vertexBuffer ); // load process

      fileName = "pixel.cso";
      Buffer pixelBuffer; // pixel shader buffer
      loadShader ( fileName, &pixelBuffer );

      // Direct3D interface for vertex shaders: vertex shader creation
      // purpose: invoking the HLSL shaders for drawing the 3D models already on the GPU
      hR = device->CreateVertexShader ( vertexBuffer.buffer,
                                        vertexBuffer.size, &vertexShader );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Creation of vertex shader failed!" + entryPoint );
        return false;
      }

      // Direct3D interface for pixel shaders: pixel shader creation
      hR = device->CreatePixelShader ( pixelBuffer.buffer,
                                       pixelBuffer.size, &pixelShader );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Creation of pixel shader failed!" + entryPoint );
        return false;
      }

      // input layout creation (how to handle the defined vertices)
      // the interface holds definition of how to feed vertex data into the input-assembler stage of the graphics rendering pipeline
      //unsigned int numElements { sizeof ( *polygonLayout ) / sizeof ( polygonLayout [0] ) };
      hR = device->CreateInputLayout ( polygonLayout, 2,
                                       vertexBuffer.buffer,
                                       vertexBuffer.size, &inputLayout );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Creation of input layout failed!" + entryPoint );
        return false;
      }

      vertexBuffer.release ();
      pixelBuffer.release ();

    } else
    {

      ID3D10Blob* errorMsg; // HLSL compilation errors container
      std::string errorStr; // the same in string
      ID3D10Blob* vertexBuffer; // texture buffer

      // directly compile the shader into buffer (DirectX APIs)
      hR = D3DCompileFromFile ( L"./graphics/vertexT.hlsl", nullptr, nullptr,
                                "main", "vs_4_1", D3D10_SHADER_ENABLE_STRICTNESS,
                                0, &vertexBuffer, &errorMsg );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Compilation of texture vertex shader file failed!" + entryPoint );
        if (errorMsg)
        {
          PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                    Converter::strConverter ( ( char*) errorMsg->GetBufferPointer () )
                                                    + entryPoint );
          errorStr = ( char*) errorMsg->GetBufferPointer ();
          errorMsg->Release ();
        }
        return false;
      }

      // purpose: invoking the HLSL shaders for drawing the 3D models already on the GPU
      hR = device->CreateVertexShader ( vertexBuffer->GetBufferPointer (),
                                        vertexBuffer->GetBufferSize (), &vertexShader );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Creation of texture vertex shader failed!" + entryPoint );
        return false;
      }

      ID3D10Blob* pixelBuffer;
      hR = D3DCompileFromFile ( L"./graphics/pixelT.hlsl", nullptr, nullptr,
                                "main", "ps_4_1", D3D10_SHADER_ENABLE_STRICTNESS,
                                0, &pixelBuffer, &errorMsg );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Compilation of texture pixel shader file failed!" + entryPoint );
        if (errorMsg)
        {
          PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                    Converter::strConverter ( ( char*) errorMsg->GetBufferPointer () )
                                                    + entryPoint );
          errorStr = ( char*) errorMsg->GetBufferPointer ();
          errorMsg->Release ();
        }
        return false;
      }

      hR = device->CreatePixelShader ( pixelBuffer->GetBufferPointer (),
                                       pixelBuffer->GetBufferSize (), &pixelShader );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Creation of texture pixel shader failed!" + entryPoint );
        return false;
      }

      // input layout creation (how to handle the defined vertices)
      // the interface holds definition of how to feed vertex data into the input-assembler stage of the graphics rendering pipeline
      //unsigned int numElements = sizeof ( *polygonLayout ) / sizeof ( polygonLayout [0] );
      hR = device->CreateInputLayout ( polygonLayout, 2,
                                       vertexBuffer->GetBufferPointer (),
                                       vertexBuffer->GetBufferSize (), &inputLayout );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Creation of texture input layout failed!" + entryPoint );
        return false;
      }

      vertexBuffer->Release ();
      pixelBuffer->Release ();

    }

    if (sampler)
    {
      // testure sampler state creation
      hR = device->CreateSamplerState ( sampler, &samplerState );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Creation of texture sampler state failed!" + entryPoint );
        return false;
      }
    }

    return true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) + entryPoint );
    return false;
  }
};


ID3D10VertexShader* const Shader::getVertexShader ( void )
{
  return vertexShader;
};


ID3D10PixelShader* const Shader::getPixelShader ( void )
{
  return pixelShader;
};


ID3D10InputLayout* const Shader::getInputLayout ( void )
{
  return inputLayout;
};


ID3D10SamplerState** const Shader::getSamplerState ( void )
{
  return &samplerState;
};


void Shader::release ( void )
{
  try
  {
    if (vertexShader)
    {
      vertexShader->Release ();
      vertexShader = nullptr;
    }
    if (pixelShader)
    {
      pixelShader->Release ();
      pixelShader = nullptr;
    }
    if (inputLayout)
    {
      inputLayout->Release ();
      inputLayout = nullptr;
    }
    if (samplerState)
    {
      samplerState->Release ();
      samplerState = nullptr;
    }

    device = nullptr;
  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


ShaderColour::ShaderColour ( ID3D10Device1* dev ) : Shader ( dev, L"ColourShader" ), initialized ( false )
{
  try
  {

    // input layout description (passed into the shader)
      // note that the description must match to the vertex types defined in game and shader
      // teaching the GPU how to read a custom vertex structure.
      // note that to improve the rendering speed,
      // the GPU can be told what information with each vertex needs to be stored.
      // note flag D3D10_APPEND_ALIGNED_ELEMENT: the elements are one after each other,
      // therefore automatically figure the spacing out. (no need to define the offset)
    polygonLayoutDesc [0].SemanticName = "POSITION"; // what a certain value is used for
    polygonLayoutDesc [0].SemanticIndex = 0; // modifies the semantic with an integer index (multiple elements with same semantic)
    polygonLayoutDesc [0].Format = DXGI_FORMAT_R32G32B32_FLOAT; // 32 bits for each x, y and z
    polygonLayoutDesc [0].InputSlot = 0; // imput - assembler or input slot through which data is fed to GPU ( Direct3D supports sixteen input slots )
    polygonLayoutDesc [0].AlignedByteOffset = 0; // the offset between each element in the structure
    polygonLayoutDesc [0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA; // input data class for a single input slot
    polygonLayoutDesc [0].InstanceDataStepRate = 0; // for now

    polygonLayoutDesc [1].SemanticName = "COLOR"; // colour semantic
    polygonLayoutDesc [1].SemanticIndex = 0;
    polygonLayoutDesc [1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 32 bits for each x, y and z
    polygonLayoutDesc [1].InputSlot = 0;
    polygonLayoutDesc [1].AlignedByteOffset = 12; // offset: 3*4 byte of float type
    polygonLayoutDesc [1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
    polygonLayoutDesc [1].InstanceDataStepRate = 0;

    initialized = initialize ( polygonLayoutDesc, nullptr, false );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


const bool& ShaderColour::isInitialized ( void )
{
  return initialized;
};


ShaderTexture::ShaderTexture ( ID3D10Device1* dev ) : Shader ( dev, L"TextureShader" ), initialized ( false )
{
  try
  {

    polygonLayoutDesc [0].SemanticName = "POSITION";
    polygonLayoutDesc [0].SemanticIndex = 0;
    polygonLayoutDesc [0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayoutDesc [0].InputSlot = 0;
    polygonLayoutDesc [0].AlignedByteOffset = 0;
    polygonLayoutDesc [0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
    polygonLayoutDesc [0].InstanceDataStepRate = 0;

    polygonLayoutDesc [1].SemanticName = "TEXCOORD"; // texture coordinate semantic
    // note that numbered semantics are introduced here without any numbers
    polygonLayoutDesc [1].SemanticIndex = 0;
    polygonLayoutDesc [1].Format = DXGI_FORMAT_R32G32_FLOAT; // 32 bits for each U (width) and V (height)
    polygonLayoutDesc [1].InputSlot = 0;
    polygonLayoutDesc [1].AlignedByteOffset = 12; //D3D10_APPEND_ALIGNED_ELEMENT
    polygonLayoutDesc [1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
    polygonLayoutDesc [1].InstanceDataStepRate = 0;


    // tecture sampler state description (to interface with texture shader):
    // which pixel or what combination of them to use when drawing (near or far away polygon)
    // liner option: the most expensive in processing and the best visual result,
    // unisng linear interpretation for manification, magnification and mip-level sampling.
    samplerDesc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR; // the most important one
    // wrap: ensures the coordinates stay between 0.0f and 1.0f by wrapping anything already outside,
    // around and within these values
    samplerDesc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D10_COMPARISON_ALWAYS;
    samplerDesc.BorderColor [0] = 0.0f;
    samplerDesc.BorderColor [1] = 0.0f;
    samplerDesc.BorderColor [2] = 0.0f;
    samplerDesc.BorderColor [3] = 0.0f;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D10_FLOAT32_MAX;

    initialized = initialize ( polygonLayoutDesc, &samplerDesc, true, true );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


const bool& ShaderTexture::isInitialized ( void )
{
  return initialized;
};


ShaderLight::ShaderLight ( ID3D10Device1* dev ) : Shader ( dev, L"LightShader" ), initialized ( false )
{
  try
  {


    //initialized = initialize ( polygonLayoutDesc, &samplerDesc, true, true );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


const bool& ShaderLight::isInitialized ( void )
{
  return initialized;
};
