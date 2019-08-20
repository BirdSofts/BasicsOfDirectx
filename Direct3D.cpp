// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,19.07.2019</created>
/// <changed>ʆϒʅ,20.08.2019</changed>
// ********************************************************************************

#include "Direct3D.h"


ShaderBuffer::ShaderBuffer ()
{
  buffer = nullptr;
  size = 0;
};


ShaderBuffer::~ShaderBuffer ()
{
  if (buffer)
    delete buffer;
  buffer = nullptr;
};


Direct3D::Direct3D ( TheCore* coreObj ) : core ( coreObj ),
// reserve 8 bits for red, green, blue and transparency each in unsigned normalized integer
colourFormat ( DXGI_FORMAT_B8G8R8A8_UNORM ), fullscreen ( false ),
initialized ( false ), allocated ( false )

{
  try
  {

    HRESULT hResult;

    fullscreen = PointerProvider::getConfiguration ()->getSettings ().fullscreen;

    // flag: needed to get Direct2D interoperability with Direct3D resources
    unsigned int deviceFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT;

    // device creation debug option flag
#ifndef _NOT_DEBUGGING
    deviceFlags |= D3D10_CREATE_DEVICE_DEBUG; // creation with debug layer
#endif // !_NOT_DEBUGGING

    // creation of the device
    // first parameter: pointer to the present adapter on system
    D3D10_FEATURE_LEVEL1 featureLevel { D3D10_FEATURE_LEVEL_10_1 };
    hResult = D3D10CreateDevice1 ( nullptr, D3D10_DRIVER_TYPE_HARDWARE, NULL,
                                   deviceFlags, featureLevel, D3D10_1_SDK_VERSION, &device );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"The creation of Direct3D device failed!" );
      return;
    }

#ifndef _NOT_DEBUGGING
    // acquiring the device's debug layer
    // note that live report is available from Direct3D 11
    hResult = device->QueryInterface ( __uuidof(ID3D10Debug), &debug );

    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"The creation of Direct3D device debug layer failed!" );
      return;
    }
#endif // !_NOT_DEBUGGING

    // acquiring the underlying DXGI factory used to create the device (resources needs)
    hResult = device->QueryInterface ( __uuidof(IDXGIDevice1), ( void**) & dxgiDevice );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Acquiring the DXGI device failed!" );
      return;
    }

    // physical GPU identification (resources needs)
    hResult = dxgiDevice->GetParent ( __uuidof(IDXGIAdapter), ( void**) & dxgiAdapter );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Acquiring the DXGI adapter failed!" );
      return;
    }

    // retrieving the devices's factory
    hResult = dxgiAdapter->GetParent ( __uuidof(IDXGIFactory), ( void**) & dxgiFactory );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Acquiring the DXGI factory failed!" );
      return;
    }

    // filling a swap chain description structure (the type of swap chain)
    DXGI_SWAP_CHAIN_DESC descSwapC;
    descSwapC.BufferDesc.Width = 0; // back buffer size, 0: automatic adjustment
    descSwapC.BufferDesc.Height = 0; // the same
    descSwapC.BufferDesc.RefreshRate.Numerator = 0; // 0: don't care
    descSwapC.BufferDesc.RefreshRate.Denominator = 1;
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
    descSwapC.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow switching the display mode

    // swap chain creation
    hResult = dxgiFactory->CreateSwapChain ( device.Get (), &descSwapC, &swapChain );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of swap chain failed!" );
      return;
    }

    // acquire the output adapter representation
    hResult = swapChain->GetContainingOutput ( &output );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Acquiring the output device failed!" );
      return;
    }

    // two calls to enumeration the number and supported display modes:
    //-- the number of supported display modes
    // refer to MSDN for flags parameter
    hResult = output->GetDisplayModeList ( colourFormat, 0, &displayModesCount, nullptr );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Acquiring the number of supported display modes failed!" );
      return;
    }

    //-- acquiring all the supported modes
    // refer to MSDN for flags parameter
    displayModes = new (std::nothrow) DXGI_MODE_DESC [displayModesCount];
    //ZeroMemory ( supportedModes, sizeof ( DXGI_MODE_DESC ) * supportedModesNumber );
    hResult = output->GetDisplayModeList ( colourFormat, 0, &displayModesCount, displayModes );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Acquiring all the supported display modes failed!" );
      return;
    }

    for (unsigned int i = 0; i < displayModesCount; i++)
    {
      // support check for current resolution of the client window (input streamed from setting file)
      if (displayModes [i].Width == core->appWindow->getWidth ())
        if (displayModes [i].Height == core->appWindow->getHeight ())
        {
          displayMode = displayModes [i];
          displayModeIndex = i;
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

    initialized = true;
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"Direct3D is successfully initialized." );

    displayModeSetter ();
    allocateResources ();

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


const bool& Direct3D::isInitialized ()
{
  return initialized;
};


const ID3D10Device1& Direct3D::getDevice ( void )
{
  return *device.Get ();
};


const bool& Direct3D::isFullscreen ()
{
  return fullscreen;
};


void Direct3D::displayModeSetter ( void )
{
  try
  {

    HRESULT hResult;

    if (fullscreen)
    {
      // switch to fullscreen mode
      hResult = swapChain->SetFullscreenState ( true, nullptr );
      if (FAILED ( hResult ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Switching to fullscreen mode failed." );
        fullscreen = false;
        return;
      }
    } else
    {
      // switch to windowed mode
      hResult = swapChain->SetFullscreenState ( false, nullptr );
      if (FAILED ( hResult ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Switching to windowed mode failed." );
        fullscreen = true;
        return;
      }
    }

    // setting the resolution
    hResult = swapChain->ResizeTarget ( &displayMode );
    if (FAILED ( hResult ))
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
    HRESULT hResult;

    // resizing the swap chain buffers (on resize of the client window)
    // BufferCount and SwapChainFlags: 0 do not change the current
    // 0 for the next two parameters to adjust to the current client window size automatically
    // next parameter: set to DXGI_FORMAT_UNKNOWN to preserve the current
    hResult = swapChain->ResizeBuffers ( 0, 0, 0, DXGI_FORMAT_UNKNOWN, 0 );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Resizing the swap chain failed!" );
      return;
    }

    // the render target view recreation
    // obtain a pointer to the current back buffer in the swap chain
    // the zero-th buffer is accessible, since already created using flip discarding effect.
    // second parameter: interface type (most cases 2D- texture)
    // the last parameter returns a pointer to the actual back buffer
    hResult = swapChain->GetBuffer ( 0, __uuidof(ID3D10Texture2D), reinterpret_cast<void**>(rtBuffer.GetAddressOf ()) );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Acquiring the back buffer failed!" );
      return;
    }

    // first parameter: the resource for which the render target is created for
    // second parameter describes data type of the specified resource (mipmap but 0 for now)
    // the last parameter returns a pointer to the created render target view
    hResult = device->CreateRenderTargetView ( rtBuffer.Get (), NULL, &rtView );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of render target view failed!" );
      return;
    }

    // depth-stencil buffer creation
    CD3D10_TEXTURE2D_DESC descDepth;
    rtBuffer->GetDesc ( &descDepth ); // retrieves the description of the back buffer and fill! :)
    //descDepth.Width = ;
    //descDepth.Height = ;
    //descDepth.MipLevels = 1;
    //descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 bits for depth and 8 bits for stencil
    //descDepth.SampleDesc.Count = 1; // multi-sampling (anti-aliasing) match to settings of render target
    //descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D10_USAGE_DEFAULT; // value: only GPU will be reading and writing to the resource
    descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL; // how to bind to the different pipeline stages
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    // texture creation:
    // the second parameter: pointer to initial data (zero for any data, since depth-stencil buffer)
    hResult = device->CreateTexture2D ( &descDepth, nullptr, &dsBuffer );
    hResult = dsBuffer->QueryInterface ( __uuidof(IDXGISurface1), &dsSurface );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of depth-stencil buffer failed!" );
      return;
    }

    // depth-stencil view description
    D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
    descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    // depth-stencil view creation
    // the second parameter: zero to access the mipmap level 0
    hResult = device->CreateDepthStencilView ( dsBuffer.Get (), &descDSV, &dsView );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation the depth-stencil view failed!" );
      return;
    }

    // binding the depth-stencil view (for now one render target view)
    // second parameter: pointer to first element of a list of render target view pointers
    device->OMSetRenderTargets ( 1, rtView.GetAddressOf (), dsView.Get () );


    // viewport structure: set the viewport to entire back buffer (what area should be rendered to)
    D3D10_VIEWPORT viewPort;
    viewPort.TopLeftX = 0; // first four integers: viewport rectangle (relative to client window rectangle)
    viewPort.TopLeftY = 0;
    viewPort.Width = descDepth.Width;
    viewPort.Height = descDepth.Height;
    viewPort.MinDepth = 0.0f; // minimum and maximum depth buffer values
    viewPort.MaxDepth = 1.0f;
    // setting the viewport
    // the second parameter is a pointer to an array of viewports
    device->RSSetViewports ( 1, &viewPort );

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
    device->ClearRenderTargetView ( rtView.Get (), blue );
    // second parameter: the type of data to clear (obviously set to clear both depth-stencil)
    // the values are used to override the entire depth-stencil buffer with
    if (dsView)
      device->ClearDepthStencilView ( dsView.Get (), D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0 );

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

    HRESULT hResult;
    std::string fileName { "" };

    ShaderBuffer vertexBuffer; // vertex shader buffer
    ShaderBuffer pixelBuffer; // pixel shader buffer

    // shaders, introduced in several different types,
    // are used to actually render vertices or pixels to the screen in the render process,
    // controlled by the graphics rendering pipeline, which is programmed by these shaders,
    // and each of them is a small program that controls one step of the pipeline.
    // the process in nature takes vertices as input and results in fully rendered images,
    // in which each type of shader is run many times based on its different nature.
    // note that shaders are written in HLSL (High Level Shader Language),
    // Visual Studio is able to create HLSL programs, and after compiling,
    // the HLSL file is compiled into CSO (Compiled Shader Objects), usable by the running program.

    // Direct3D interface for vertex shaders: creation of the vertex shader
    fileName = "VertexShader.cso";
    loadShader ( fileName, &vertexBuffer ); // load process
    hResult = device->CreateVertexShader ( vertexBuffer.buffer, vertexBuffer.size, &vertexShader );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of vertex shader failed!" );
      return;
    }

    // Direct3D interface for pixel shaders: creation of the pixel shader interface
    fileName = "PixelShader.cso";
    loadShader ( fileName, &pixelBuffer );
    hResult = device->CreatePixelShader ( pixelBuffer.buffer, pixelBuffer.size, &pixelShader );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of pixel shader failed!" );
      return;
    }

    // setting the active vertex/pixel shaders
    device->VSSetShader ( vertexShader.Get () );
    device->PSSetShader ( pixelShader.Get () );

    // input layout description
    // teaching the GPU how to read a custom vertex structure.
    // note that to improve the rendering speed,
    // the GPU can be told what information with each vertex needs to be stored.
    // note flag D3D10_APPEND_ALIGNED_ELEMENT: introduce the elements one after each other including any packing if necessary,
    // thus no need to define the offset
    D3D10_INPUT_ELEMENT_DESC descInputE [] { {
        "POSITION", // SemanticName: (what a certain value is used for)
        0, // SemanticIndex: modifies the semantic with an integer index (multiple elements with same semantic)
        DXGI_FORMAT_R32G32B32_FLOAT, // 32 bits for each x, y and z
        0, // InputSlot: imput-assembler or input slot through which data is fed to GPU (Direct3D supports sixteen input slots)
        0, // AlignedByteOffset: the offset between each element in the structure
        D3D10_INPUT_PER_VERTEX_DATA, // InputSlotClass: input data class for a single input slot
        0}, // InstanceDataStepRate // for now
    {
      "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, // offset: 3*4 byte of float type
      D3D10_INPUT_PER_VERTEX_DATA, 0} };

    // input layout creation (how to handle the defined vertices)
    // the interface holds definition of how to feed vertex data into the input-assembler stage of the graphics rendering pipeline
    hResult = device->CreateInputLayout ( descInputE, ARRAYSIZE ( descInputE ),
                                          vertexBuffer.buffer, vertexBuffer.size,
                                          &inputLayout );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of input layout failed!" );
      return;
    }

    // setting the active input layout
    device->IASetInputLayout ( inputLayout.Get () );

    allocated = true;
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"Direct3D resources is successfully allocated." );

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

    // SyncInterval parameter: the way a frame is synchronized with VBLANK:
    // in flip mode: the n = 0 sets the DirectX to cancel the remaining time of previously rendered scene
    // and discard this frame if a newer frame is on the queue. (screen tearing might occur)
    // the n = 1 to 4 values: synchronize the presentation after n-th vertical blank.
    // the second parameter: not waiting for v-sync.
    HRESULT hResult { swapChain->Present ( 0, DXGI_PRESENT_DO_NOT_WAIT ) };

    if ((FAILED ( hResult )) &&
      (hResult != DXGI_ERROR_WAS_STILL_DRAWING)) // occurs, if the calling thread is blocked
    {
      PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), L"mainThread",
                                                L"The presentation of the scene failed!" );
    }

    // rebind: the process is needed after each call to present, since in flip and discard mode the view targets are released.
    if (dsView)
      device->OMSetRenderTargets ( 1, rtView.GetAddressOf (), dsView.Get () );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};
