// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,26.08.2019</changed>
// ********************************************************************************

#include "Direct3D.h"


ShaderBuffer::ShaderBuffer ( void )
{
  buffer = nullptr;
  size = 0;
};


ShaderBuffer::~ShaderBuffer ( void )
{
  if (buffer)
    delete buffer;
  buffer = nullptr;
};


Direct3D::Direct3D ( TheCore* coreObj ) : core ( coreObj ),
// reserve 8 bits for red, green, blue and transparency each in unsigned normalized integer
colourFormat ( DXGI_FORMAT_B8G8R8A8_UNORM ),
displayModesCount ( 0 ), displayModeIndex ( 0 ), videoCardMemory ( 0 ), videoCardDescription ( L"" ),
fullscreen ( false ), vSync ( false ), initialized ( false ), allocated ( false )
{
  try
  {

    HRESULT hR; // functions return value
    unsigned long rC { 0 }; // reference counts

    fullscreen = PointerProvider::getConfiguration ()->getSettings ().fullscreen;

    //vSync = PointerProvider::getConfiguration ()->getSettings ().vsync;

    // DXGI (DirectX graphics interface) factory creation
    IDXGIFactory1* dxgiFactory; // DXGI factory
    hR = CreateDXGIFactory1 ( __uuidof(IDXGIFactory1), ( void**) & dxgiFactory );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of DXGI factory failed!" );
      return;
    }

    // adapter creation for physical graphics interface using DXGI factory (0: primary device)
    IDXGIAdapter1* dxgiAdapter; // DXGI adapter
    hR = dxgiFactory->EnumAdapters1 ( 0, &dxgiAdapter );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of DXGI adapter failed!" );
      return;
    }

    // primary output adapter (monitor) enumeration
    IDXGIOutput* dxgiOutput; // DXGI output
    hR = dxgiAdapter->EnumOutputs ( 0, &dxgiOutput );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Acquiring the output device failed!" );
      return;
    }

    // two calls to enumerate the number and supported monitor display modes for the colour format:
    // -- the number of supported display modes (refer to MSDN for flags parameter)
    hR = dxgiOutput->GetDisplayModeList ( colourFormat, DXGI_ENUM_MODES_INTERLACED, &displayModesCount, nullptr );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Acquiring the number of supported display modes failed!" );
      return;
    }

    // -- acquiring all the supported display modes for current monitor / video card combination (refer to MSDN for flags parameter)
    displayModes = new (std::nothrow) DXGI_MODE_DESC [displayModesCount];
    if (!displayModes)
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Dynamic memory allocation for supported display modes failed!" );
      return;
    }
    hR = dxgiOutput->GetDisplayModeList ( colourFormat, DXGI_ENUM_MODES_INTERLACED, &displayModesCount, displayModes );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Acquiring all the supported display modes failed!" );
      return;
    }

    unsigned int numerator { 0 }, denominator { 0 };
    for (unsigned int i = 0; i < displayModesCount; i++)
    {
      // support check for current resolution of the client window (input streamed from setting file)
      if (displayModes [i].Width == core->appWindow->getWidth ())
        if (displayModes [i].Height == core->appWindow->getHeight ())
        {
          displayMode = displayModes [i];
          displayModeIndex = i;
          numerator = displayModes [i].RefreshRate.Numerator;
          denominator = displayModes [i].RefreshRate.Denominator;
          break;
        } else
        {
          // not supported: set to the lowest supported resolution
          displayMode = displayModes [0];
          displayModeIndex = 0;

          PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), L"mainThread",
                                                    L"The chosen resolution is not supported!" );

          // rewrite a not valid configurations with defaults: the file is probably modified from outside of the application
          if (!PointerProvider::getConfiguration ()->apply ( PointerProvider::getConfiguration ()->getDefaults () ))
          {
            PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                      L"Rewriting the Configuration file with default settings failed." );
          }
        }
    }
    rC = dxgiOutput->Release ();

    // acquiring the video card description
    DXGI_ADAPTER_DESC1 descDxgiAdapter;
    hR = dxgiAdapter->GetDesc1 ( &descDxgiAdapter );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Acquiring the description of video card failed!" );
      return;
    }
    videoCardMemory = ( unsigned int) (descDxgiAdapter.DedicatedVideoMemory / 1024 / 1024);
    videoCardDescription = descDxgiAdapter.Description;
    rC = dxgiAdapter->Release ();

    // flag: needed to get Direct2D interoperability with Direct3D resources
    unsigned int deviceFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT;

    // device creation debug option flag
#ifndef _NOT_DEBUGGING
    deviceFlags |= D3D10_CREATE_DEVICE_DEBUG; // creation with debug layer
#endif // !_NOT_DEBUGGING

    // creation of the device
    // first parameter: pointer to the present adapter on system
    D3D10_FEATURE_LEVEL1 featureLevel { D3D10_FEATURE_LEVEL_10_1 };
    hR = D3D10CreateDevice1 ( nullptr, D3D10_DRIVER_TYPE_HARDWARE, NULL,
                              deviceFlags, featureLevel, D3D10_1_SDK_VERSION, &device );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"The creation of Direct3D device failed!" );
      return;
    }

#ifndef _NOT_DEBUGGING
    // acquiring the device's debug layer
    // note that live report is available from Direct3D 11
    hR = device->QueryInterface ( __uuidof(ID3D10Debug), &debug );

    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"The creation of Direct3D device debug layer failed!" );
      return;
    }
#endif // !_NOT_DEBUGGING

    // filling a swap chain description structure (the type of swap chain)
    DXGI_SWAP_CHAIN_DESC descSwapC;
    descSwapC.BufferDesc.Width = 0; // back buffer size, 0: automatic adjustment
    descSwapC.BufferDesc.Height = 0; // the same
    if (vSync)
    {
      descSwapC.BufferDesc.RefreshRate.Numerator = numerator;
      descSwapC.BufferDesc.RefreshRate.Denominator = denominator;
    } else
    {
      descSwapC.BufferDesc.RefreshRate.Numerator = 0;
      descSwapC.BufferDesc.RefreshRate.Denominator = 1;
    }
    descSwapC.BufferDesc.Format = colourFormat; // display format
    descSwapC.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // scan-line drawing
    descSwapC.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // image size adjustment to back buffer resolution
    // number of multi samplings per pixel and image quality (1 and 0: disable multi sampling (no anti-aliasing))
    descSwapC.SampleDesc.Count = 1;
    descSwapC.SampleDesc.Quality = 0;
    descSwapC.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // back buffer as render output target
    descSwapC.BufferCount = 3; // including the front buffer (one front buffer and two back buffers)
    descSwapC.OutputWindow = core->getHandle (); // handle to main window
    descSwapC.Windowed = true; // recommendation: windowed creation and switch to full screen
    // flip (in windowed mode: blit) and discard the content of back buffer after presentation
    descSwapC.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    descSwapC.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow switching the display mode (advanced)

    // swap chain creation
    hR = dxgiFactory->CreateSwapChain ( device.Get (), &descSwapC, &swapChain );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of swap chain failed!" );
      return;
    }
    rC = dxgiFactory->Release ();

    initialized = true;
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"Direct3D is successfully initialized." );

    displayModeSetter ();
    allocateResources ();

    if (!allocated)
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Allocation of Direct3D resources failed." );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


const bool& Direct3D::isInitialized ( void )
{
  return initialized;
};


const ID3D10Device1& Direct3D::getDevice ( void )
{
  return *device.Get ();
};


const bool& Direct3D::isFullscreen ( void )
{
  return fullscreen;
};


void Direct3D::displayModeSetter ( void )
{
  try
  {

    HRESULT hR;

    if (fullscreen)
    {
      // switch to fullscreen mode
      hR = swapChain->SetFullscreenState ( true, nullptr );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Switching to fullscreen mode failed." );
        fullscreen = false;
        return;
      }
    } else
    {
      // switch to windowed mode
      hR = swapChain->SetFullscreenState ( false, nullptr );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Switching to windowed mode failed." );
        fullscreen = true;
        return;
      }
    }

    // setting the resolution
    hR = swapChain->ResizeTarget ( &displayMode );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Setting a supported resolution failed." );
    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void Direct3D::allocateResources ( void )
{
  try
  {

    allocated = false;
    HRESULT hR;
    unsigned long rC { 0 };

    // resizing the swap chain buffers (on resize of the client window)
    // BufferCount and SwapChainFlags: 0 do not change the current
    // 0 for the next two parameters to adjust to the current client window size automatically
    // next parameter: set to DXGI_FORMAT_UNKNOWN to preserve the current
    hR = swapChain->ResizeBuffers ( 0, 0, 0, DXGI_FORMAT_UNKNOWN, 0 );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Resizing the swap chain failed!" );
      return;
    }

    // obtain a pointer to the current back buffer of swap chain
    // the zero-th buffer is accessible, since already created using flip discarding effect.
    // second parameter: interface type (most cases 2D- texture)
    // the last parameter returns a pointer to the actual back buffer
    ID3D10Texture2D* rtBuffer; // render target view buffer
    hR = swapChain->GetBuffer ( 0, __uuidof(ID3D10Texture2D), ( LPVOID*) & rtBuffer );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Acquiring the back buffer failed!" );
      return;
    }

    // render target view creation
    // first parameter: the resource for which the render target is created for
    // second parameter describes data type of the specified resource (mipmap but 0 for now)
    // the last parameter returns a pointer to the created render target view
    hR = device->CreateRenderTargetView ( rtBuffer, nullptr, &rTview );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of render target view failed!" );
      return;
    }

    // depth-stencil buffer creation
    CD3D10_TEXTURE2D_DESC descDepthBuffer;
    rtBuffer->GetDesc ( &descDepthBuffer ); // retrieves the description of the back buffer and fill! :)
    //descDepth.Width = ;
    //descDepth.Height = ;
    //descDepth.MipLevels = 1;
    //descDepth.ArraySize = 1;
    descDepthBuffer.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 bits for depth and 8 bits for stencil
    //descDepth.SampleDesc.Count = 1; // multi-sampling (anti-aliasing) match to settings of render target
    //descDepth.SampleDesc.Quality = 0;
    descDepthBuffer.Usage = D3D10_USAGE_DEFAULT; // value: only GPU will be reading and writing to the resource
    descDepthBuffer.BindFlags = D3D10_BIND_DEPTH_STENCIL; // how to bind to the different pipeline stages
    descDepthBuffer.CPUAccessFlags = 0;
    descDepthBuffer.MiscFlags = 0;
    // texture creation:
    // the second parameter: pointer to initial data (zero for any data, since depth-stencil buffer)
    hR = device->CreateTexture2D ( &descDepthBuffer, nullptr, &dSbuffer );
    //hR = dsBuffer->QueryInterface ( __uuidof(IDXGISurface1), &dsSurface );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of depth-stencil buffer failed!" );
      return;
    }
    rC = rtBuffer->Release ();

    // depth-stencil state description
    D3D10_DEPTH_STENCIL_DESC descDepthStencil;
    descDepthStencil.DepthEnable = true;
    descDepthStencil.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
    descDepthStencil.DepthFunc = D3D10_COMPARISON_LESS;
    descDepthStencil.StencilEnable = true;
    descDepthStencil.StencilReadMask = 0xFF;
    descDepthStencil.StencilWriteMask = 0xFF;
    // stencil operations (if pixel is front facing)
    descDepthStencil.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
    descDepthStencil.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
    descDepthStencil.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    descDepthStencil.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
    // stencil operations (if pixel is front facing)
    descDepthStencil.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
    descDepthStencil.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
    descDepthStencil.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    descDepthStencil.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

    // depth stencil state creation
    hR = device->CreateDepthStencilState ( &descDepthStencil, &dSstate );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of depth-stencil state failed!" );
      return;
    }
    // set the active depth stencil state
    device->OMSetDepthStencilState ( dSstate.Get (), 1 );

    // depth-stencil view description
    D3D10_DEPTH_STENCIL_VIEW_DESC descDepthStencilView;
    descDepthStencilView.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepthStencilView.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    descDepthStencilView.Texture2D.MipSlice = 0;
    // depth-stencil view creation
    // the second parameter: zero to access the mipmap level 0
    hR = device->CreateDepthStencilView ( dSbuffer.Get (), &descDepthStencilView, &dSview );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation the depth-stencil view failed!" );
      return;
    }

    // binding render target view and depth-stencil view to output render pipeline (for now one render target view)
    // second parameter: pointer to first element of a list of render target view pointers
    device->OMSetRenderTargets ( 1, rTview.GetAddressOf (), dSview.Get () );

    // rasterizer description (determines how and what polygons will be drawn)
    D3D10_RASTERIZER_DESC descRasterizer;
    descRasterizer.AntialiasedLineEnable = false;
    descRasterizer.CullMode = D3D10_CULL_BACK;
    descRasterizer.DepthBias = 0;
    descRasterizer.DepthBiasClamp = 0.0f;
    descRasterizer.DepthClipEnable = true;
    descRasterizer.FillMode = D3D10_FILL_SOLID;
    descRasterizer.FrontCounterClockwise = false;
    descRasterizer.MultisampleEnable = false;
    descRasterizer.ScissorEnable = false;
    descRasterizer.SlopeScaledDepthBias = 0.0f;
    // rasterizer creation
    hR = device->CreateRasterizerState ( &descRasterizer, &rasterizerState );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation the rasterizer state failed!" );
      return;
    }
    // set the active rasterizer state
    device->RSSetState ( rasterizerState.Get () );

    // viewport structure: set the viewport to entire back buffer (what area should be rendered to)
    D3D10_VIEWPORT viewPort;
    viewPort.Width = descDepthBuffer.Width;
    viewPort.Height = descDepthBuffer.Height;
    viewPort.MinDepth = 0.0f; // minimum and maximum depth buffer values
    viewPort.MaxDepth = 1.0f;
    viewPort.TopLeftX = 0; // first four integers: viewport rectangle (relative to client window rectangle)
    viewPort.TopLeftY = 0;
    // setting the viewport
    // the second parameter is a pointer to an array of viewports
    device->RSSetViewports ( 1, &viewPort );

    // projection matrix setup
    float viewField { DirectX::XM_PI / 4.0f };
    float screenAspect { ( float) viewPort.Width / ( float) viewPort.Height };
    // projection matrix creation for 3D rendering
    matrixProjection = DirectX::XMMatrixPerspectiveFovLH ( viewField, screenAspect, screenNear, screenDepth );
    // word matrix initialization using identity matrix
    matrixWorld = DirectX::XMMatrixIdentity ();
    // orthographic projection matrix creation for 2D rendering
    matrixOrtho = DirectX::XMMatrixOrthographicLH ( ( float) viewPort.Width, ( float) viewPort.Height, screenNear, screenDepth );

    clearBuffers ();
    initializePipeline ();

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void Direct3D::clearBuffers ( void )
{
  try
  {

    const float blue [] { 0.11f, 0.33f, 0.55f, 1.0f };
    // filling the entire back buffer with a single colour
    device->ClearRenderTargetView ( rTview.Get (), blue );
    // second parameter: the type of data to clear (obviously set to clear both depth-stencil)
    // the values are used to override the entire depth-stencil buffer with
    if (dSview)
      device->ClearDepthStencilView ( dSview.Get (), D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0 );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void Direct3D::loadShader ( std::string& fileName, ShaderBuffer* csoBuffer )
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


void Direct3D::initializePipeline ( void )
{
  try
  {

    // load and encapsulate .cso shaders into usable shader objects

    HRESULT hR;
    //std::string fileName { "" };

    //ShaderBuffer vertexShaderBuffer; // vertex shader buffer
    //ShaderBuffer pixelShaderBuffer; // pixel shader buffer

    // shaders, introduced in several different types,
    // are used to actually render vertices or pixels to the screen in the render process,
    // controlled by the graphics rendering pipeline, which is programmed by these shaders,
    // and each of them is a small program that controls one step of the pipeline.
    // the process in nature takes vertices as input and results in fully rendered images,
    // in which each type of shader is run many times based on its different nature.
    // note that shaders are written in HLSL (High Level Shader Language),
    // Visual Studio is able to create HLSL programs, and after compiling,
    // the HLSL file is compiled into CSO (Compiled Shader Objects), usable by the running program.

    // Direct3D interface for vertex shaders: vertex shader creation

    //fileName = "VertexShader.cso";
    //loadShader ( fileName, &vertexBuffer ); // load process

    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* error;
    hR = D3DCompileFromFile ( L"./graphics/vertex.hlsl", nullptr, nullptr,
                              "main", "vs_4_1", D3D10_SHADER_ENABLE_STRICTNESS,
                              0, &vertexShaderBuffer, &error );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Compilation of vertex shader file failed!" );
      return;
    }

    hR = device->CreateVertexShader ( vertexShaderBuffer->GetBufferPointer (),
                                      vertexShaderBuffer->GetBufferSize (), &vertexShader );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of vertex shader failed!" );
      return;
    }

    // Direct3D interface for pixel shaders: pixel shader creation

    //fileName = "PixelShader.cso";
    //loadShader ( fileName, &pixelShaderBuffer );

    ID3D10Blob* pixelShaderBuffer;
    hR = D3DCompileFromFile ( L"./graphics/pixel.hlsl", nullptr, nullptr,
                              "main", "ps_4_1", D3D10_SHADER_ENABLE_STRICTNESS,
                              0, &pixelShaderBuffer, &error );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Compilation of pixel shader file failed!" );
      return;
    }

    hR = device->CreatePixelShader ( pixelShaderBuffer->GetBufferPointer (),
                                     pixelShaderBuffer->GetBufferSize (), &pixelShader );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of pixel shader failed!" );
      return;
    }

    // setting the active vertex/pixel shaders
    device->VSSetShader ( vertexShader.Get () );
    device->PSSetShader ( pixelShader.Get () );

    // input layout description (passed into the shader)
    // note that the description must match to the vertex types defined in game and shader
    // teaching the GPU how to read a custom vertex structure.
    // note that to improve the rendering speed,
    // the GPU can be told what information with each vertex needs to be stored.
    // note flag D3D10_APPEND_ALIGNED_ELEMENT: introduce the elements one after each other including any packing if necessary,
    // thus no need to define the offset
    D3D10_INPUT_ELEMENT_DESC polygonLayout [2];
    polygonLayout->SemanticName = "POSITION"; // what a certain value is used for
    polygonLayout->SemanticIndex = 0; // modifies the semantic with an integer index (multiple elements with same semantic)
    polygonLayout->Format = DXGI_FORMAT_R32G32B32_FLOAT; // 32 bits for each x, y and z
    polygonLayout->InputSlot = 0; // imput - assembler or input slot through which data is fed to GPU ( Direct3D supports sixteen input slots )
    polygonLayout->AlignedByteOffset = 0; // the offset between each element in the structure
    polygonLayout->InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA; // input data class for a single input slot
    polygonLayout->InstanceDataStepRate = 0; // for now

    polygonLayout [1] = D3D10_INPUT_ELEMENT_DESC
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
      D3D10_APPEND_ALIGNED_ELEMENT, // offset: 3*4 byte of float type
      D3D10_INPUT_PER_VERTEX_DATA, 0 };

    // input layout creation (how to handle the defined vertices)
    // the interface holds definition of how to feed vertex data into the input-assembler stage of the graphics rendering pipeline
    unsigned int numElements { sizeof ( polygonLayout ) / sizeof ( polygonLayout [0] ) };
    hR = device->CreateInputLayout ( polygonLayout, numElements,
                                     vertexShaderBuffer->GetBufferPointer (),
                                     vertexShaderBuffer->GetBufferSize (), &inputLayout );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of input layout failed!" );
      return;
    }

    // setting the active input layout
    device->IASetInputLayout ( inputLayout.Get () );

    vertexShaderBuffer->Release ();
    pixelShaderBuffer->Release ();
    if (error)
      error->Release ();

    // dynamic matrix constant buffer description (introduced in vertex shader)
    D3D10_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof ( MatrixBuffer );
    matrixBufferDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;

    // matrix constant buffer creation (usable to access vertex shader constant buffer)
    hR = device->CreateBuffer ( &matrixBufferDesc, nullptr, &matrixBuffer );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of matrix buffer failed!" );
      return;
    }

    allocated = true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void Direct3D::renderMatrices ( void )
{
  try
  {

    HRESULT hR;

    void* mappedResource;
    MatrixBuffer* dataPtr;

    // transpose process (preparation for shader)
    DirectX::XMMATRIX world, view, projection;
    world = DirectX::XMMatrixTranspose ( matrixWorld );
    view = DirectX::XMMatrixTranspose ( core->camera->matrixView );
    projection = DirectX::XMMatrixTranspose ( matrixProjection );

    // prepare for write (lock the constant buffer)
    hR = matrixBuffer->Map ( D3D10_MAP_WRITE_DISCARD, 0, &mappedResource );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Mapping the matrix buffer failed!" );
      return;
    }

    // initialize the pointer to the data already in the constant buffer
    dataPtr = ( MatrixBuffer*) mappedResource;

    // copy the matrices to the constant buffer
    dataPtr->world = world;
    dataPtr->view = view;
    dataPtr->projection = projection;

    // unlock and make the buffer usable
    matrixBuffer->Unmap ();

    // activate the updated constant matrix buffer in the vertex shader
    // first parameter: position of the constant buffer in the vertex shader
    device->VSSetConstantBuffers ( 0, 1, matrixBuffer.GetAddressOf () );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void Direct3D::present ( void )
{
  try
  {

    HRESULT hR;

    // SyncInterval parameter: the way a frame is synchronized with VBLANK:
    // in flip mode: the n = 0 sets the DirectX to cancel the remaining time of previously rendered scene
    // and discard this frame if a newer frame is on the queue. (screen tearing might occur)
    // the n = 1 to 4 values: synchronize the presentation after n-th vertical blank.
    // the second parameter: not waiting for v-sync.
    if (vSync)
      hR = { swapChain->Present ( 1, DXGI_PRESENT_DO_NOT_WAIT ) }; // consider screen refresh rate
    else
      hR = { swapChain->Present ( 0, DXGI_PRESENT_DO_NOT_WAIT ) }; // as fast as possible

    if ((FAILED ( hR )) &&
      (hR != DXGI_ERROR_WAS_STILL_DRAWING)) // occurs, if the calling thread is blocked
    {
      PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), L"mainThread",
                                                L"The presentation of the scene failed!" );
    }

    // rebind: the process is needed after each call to present, since in flip and discard mode the view targets are released.
    if (dSview)
      device->OMSetRenderTargets ( 1, rTview.GetAddressOf (), dSview.Get () );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};
