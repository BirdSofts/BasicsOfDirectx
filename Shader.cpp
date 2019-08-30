// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.08.2019</created>
/// <changed>ʆϒʅ,31.08.2019</changed>
// ********************************************************************************


#include "Shader.h"


ShaderBuffer::ShaderBuffer ( void )
{
  buffer = nullptr;
  size = 0;
};


ShaderBuffer::~ShaderBuffer ( void )
{
  Release ();
};


void ShaderBuffer::Release ( void )
{
  if (buffer)
    delete buffer;
  buffer = nullptr;
};


Shader::Shader ( Direct3D* d3dObj ) : initialized ( false ), d3d ( d3dObj )
{
  initialized = true;
};


const bool& Shader::isInitialized ( void )
{
  return initialized;
};


void Shader::initializePipeline ( void )
{
  try
  {

    // load and encapsulate .cso shaders into usable shader objects

    HRESULT hR;

    std::string fileName { "" };

    ShaderBuffer vertexBuffer; // vertex shader buffer

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

    // Direct3D interface for vertex shaders: vertex shader creation
    fileName = "vertex.cso"; // compiled by VisualStudio
    loadShader ( fileName, &vertexBuffer ); // load process
    // purpose: invoking the HLSL shaders for drawing the 3D models already on the GPU
    hR = d3d->device->CreateVertexShader ( vertexBuffer.buffer,
                                           vertexBuffer.size, &vertexShader );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of vertex shader failed!" );
      return;
    }

    // Direct3D interface for pixel shaders: pixel shader creation
    ShaderBuffer pixelBuffer; // pixel shader buffer
    fileName = "pixel.cso"; // compiled by VisualStudio
    loadShader ( fileName, &pixelBuffer );
    hR = d3d->device->CreatePixelShader ( pixelBuffer.buffer,
                                          pixelBuffer.size, &pixelShader );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of pixel shader failed!" );
      return;
    }

    // input layout description (passed into the shader)
    // note that the description must match to the vertex types defined in game and shader
    // teaching the GPU how to read a custom vertex structure.
    // note that to improve the rendering speed,
    // the GPU can be told what information with each vertex needs to be stored.
    // note flag D3D10_APPEND_ALIGNED_ELEMENT: the elements are one after each other,
    // therefore automatically figure the spacing out. (no need to define the offset)
    D3D10_INPUT_ELEMENT_DESC polygonLayoutDesc [2];
    polygonLayoutDesc [0].SemanticName = "POSITION"; // what a certain value is used for
    polygonLayoutDesc [0].SemanticIndex = 0; // modifies the semantic with an integer index (multiple elements with same semantic)
    polygonLayoutDesc [0].Format = DXGI_FORMAT_R32G32B32_FLOAT; // 32 bits for each x, y and z
    polygonLayoutDesc [0].InputSlot = 0; // imput - assembler or input slot through which data is fed to GPU ( Direct3D supports sixteen input slots )
    polygonLayoutDesc [0].AlignedByteOffset = 0; // the offset between each element in the structure
    polygonLayoutDesc [0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA; // input data class for a single input slot
    polygonLayoutDesc [0].InstanceDataStepRate = 0; // for now

    polygonLayoutDesc [1] = D3D10_INPUT_ELEMENT_DESC
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
      D3D10_APPEND_ALIGNED_ELEMENT, // offset: 3*4 byte of float type
      D3D10_INPUT_PER_VERTEX_DATA, 0 };

    // input layout creation (how to handle the defined vertices)
    // the interface holds definition of how to feed vertex data into the input-assembler stage of the graphics rendering pipeline
    unsigned int numElements { sizeof ( polygonLayoutDesc ) / sizeof ( polygonLayoutDesc [0] ) };
    hR = d3d->device->CreateInputLayout ( polygonLayoutDesc, numElements,
                                          vertexBuffer.buffer,
                                          vertexBuffer.size, &inputLayout );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of input layout failed!" );
      return;
    }

    vertexBuffer.Release ();
    pixelBuffer.Release ();



    ID3D10Blob* errorMsg; // HLSL compilation errors container
    std::string errorStr; // the same in string
    ID3D10Blob* vertexTBuffer; // texture buffer

    // directly compile the shader into buffer (DirectX APIs)
    hR = D3DCompileFromFile ( L"./graphics/vertexT.hlsl", nullptr, nullptr,
                              "main", "vs_4_1", D3D10_SHADER_ENABLE_STRICTNESS,
                              0, &vertexTBuffer, &errorMsg );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Compilation of texture vertex shader file failed!" );
      if (errorMsg)
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  Converter::strConverter ( ( char*) errorMsg->GetBufferPointer () ) );
        errorStr = ( char*) errorMsg->GetBufferPointer ();
        errorMsg->Release ();
      }
      return;
    }

    // purpose: invoking the HLSL shaders for drawing the 3D models already on the GPU
    hR = d3d->device->CreateVertexShader ( vertexTBuffer->GetBufferPointer (),
                                           vertexTBuffer->GetBufferSize (), &vertexShaderT );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of texture vertex shader failed!" );
      return;
    }

    ID3D10Blob* pixelTBuffer;
    hR = D3DCompileFromFile ( L"./graphics/pixelT.hlsl", nullptr, nullptr,
                              "main", "ps_4_1", D3D10_SHADER_ENABLE_STRICTNESS,
                              0, &pixelTBuffer, &errorMsg );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Compilation of texture pixel shader file failed!" );
      if (errorMsg)
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  Converter::strConverter ( ( char*) errorMsg->GetBufferPointer () ) );
        errorStr = ( char*) errorMsg->GetBufferPointer ();
        errorMsg->Release ();
      }
      return;
    }

    hR = d3d->device->CreatePixelShader ( pixelTBuffer->GetBufferPointer (),
                                          pixelTBuffer->GetBufferSize (), &pixelShaderT );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of texture pixel shader failed!" );
      return;
    }

    polygonLayoutDesc [1].SemanticName = "TEXCOORD"; // texture coordinate semantic
    // note that numbered semantics are introduced here without any numbers
    polygonLayoutDesc [1].Format = DXGI_FORMAT_R32G32_FLOAT; // 32 bits for each U (width) and V (height)

    // input layout creation (how to handle the defined vertices)
    // the interface holds definition of how to feed vertex data into the input-assembler stage of the graphics rendering pipeline
    numElements = sizeof ( polygonLayoutDesc ) / sizeof ( polygonLayoutDesc [0] );
    hR = d3d->device->CreateInputLayout ( polygonLayoutDesc, numElements,
                                          vertexTBuffer->GetBufferPointer (),
                                          vertexTBuffer->GetBufferSize (), &inputLayoutT );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of texture input layout failed!" );
      return;
    }

    vertexTBuffer->Release ();
    pixelTBuffer->Release ();



    // dynamic matrix constant buffer description
    // purpose: to access internal variables introduced in vertex shader
    D3D10_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof ( MatrixBuffer );
    matrixBufferDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;

    // matrix constant buffer creation (usable to access vertex shader constant buffer)
    hR = d3d->device->CreateBuffer ( &matrixBufferDesc, nullptr, &matrixBuffer );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of matrix buffer failed!" );
      return;
    }



    D3D10_SAMPLER_DESC samplerDesc; // tecture sampler state description
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

    // testure sampler state creation
    hR = d3d->device->CreateSamplerState ( &samplerDesc, &samplerState );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of texture sampler state failed!" );
      return;
    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void Shader::loadShader ( std::string& fileName, ShaderBuffer* csoBuffer )
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
      csoFile.seekg ( 0, std::ios::beg );
      csoFile.read ( reinterpret_cast<char*>(csoBuffer->buffer), csoBuffer->size );
      csoFile.close ();
    } else
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Loading shader form compiled file failed!" );
    }
  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};
