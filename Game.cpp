// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,24.07.2019</created>
/// <changed>ʆϒʅ,20.08.2019</changed>
// ********************************************************************************

#include "Game.h"
#include "Shared.h"


GameWrapper::GameWrapper ( HINSTANCE& h_instance ) : initialized ( false ), allocated ( false )
{
  try
  {

    // the game framework instantiation
    core = new (std::nothrow) TheCore ( h_instance, this );

    if (!core->isInitialized ())
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Initialization of framework failed!" );
    }

    mappedLine.pData = nullptr;
    mappedLine.RowPitch = 0;

    initialized = true;
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"The game is successfully initialized." );

    allocateResources ();

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void GameWrapper::allocateResources ( void )
{
  try
  {

    allocated = false;

    // a triangle
    Vertex triangles [] {
      {-0.95f, 0.0f, 0.0f, 0.13f, 0.13f, 0.13f},
      {-0.9f, -0.12f, 0.0f, 0.53f, 0.53f, 0.53f},
      {-1.0f, -0.12f, 0.0f, 0.93f, 0.93f, 0.93f},

      {-0.75f, 0.0f, 0.0f, 0.13f, 0.13f, 0.13f},
      {-0.7f, -0.12f, 0.0f, 0.13f, 0.13f, 0.13f},
      {-0.8f, -0.12f, 0.0f, 0.13f, 0.13f, 0.13f},

      {-0.55f, 0.2f,  0.0f, 0.13f, 0.13f, 0.13f},
      {-0.5f, -0.12f, 0.0f, 00.53f, 0.53f, 0.53f},
      {-0.6f, -0.12f, 0.0f, 0.93f, 0.93f, 0.93f}
    };

    // buffer description
    D3D10_BUFFER_DESC descBufferTriangle;
    descBufferTriangle.ByteWidth = sizeof ( Vertex ) * ARRAYSIZE ( triangles ); // buffer size
    descBufferTriangle.Usage = D3D10_USAGE_DEFAULT; // default: only GPC can read and write
    descBufferTriangle.BindFlags = D3D10_BIND_VERTEX_BUFFER; // how to bound to graphics pipeline
    descBufferTriangle.CPUAccessFlags = 0; // CPU access
    //descBufferTriangle.MiscFlags = D3D10_RESOURCE_MISC_GDI_COMPATIBLE;
    descBufferTriangle.MiscFlags = 0; // for now
    //descBufferTriangle.StructureByteStride = 0; // Direct3D 11: structured buffer (the size of each element)

    // data, with which the buffer is initialized
    D3D10_SUBRESOURCE_DATA subResourceDateTriangle = {
      triangles, // pointer to data in system memory (copy to GPU)
      0, // distance between the lines of the texture in bytes (not needed for vertex buffer)
      0 }; // distance between the depth levels in bytes (not needed for vertex buffer)

    // vertex buffer: purpose: maintain system and video memory
    // note E_OUTOFMEMORY: self-explanatory
    if (FAILED ( core->d3d->device->CreateBuffer ( &descBufferTriangle, &subResourceDateTriangle, &vertexBufferTriangle ) ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of triangle vertex buffer failed!" );
      return;
    }

    Vertex line [] {
      {-.2f, 0.0f, 0.0f, 0.13f, 0.13f, 0.13f},
      {0.2f, 0.0f, 0.0f, 0.13f, 0.13f, 0.13f},
    };

    // buffer description
    D3D10_BUFFER_DESC descBufferLine;
    descBufferLine.ByteWidth = sizeof ( Vertex ) * ARRAYSIZE ( line );
    descBufferLine.Usage = D3D10_USAGE_DYNAMIC; // data is going to change (GPU read only, CPU write only)
    descBufferLine.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    descBufferLine.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE; // mappable data for write access through CPU
    //descBufferLine.MiscFlags = D3D10_RESOURCE_MISC_GDI_COMPATIBLE;
    descBufferLine.MiscFlags = 0;
    //descBufferLine.StructureByteStride = 0; // Direct3D 11

    // data, with which the buffer is initialized
    D3D10_SUBRESOURCE_DATA subResourceDateLine = { line, 0, 0 };

    if (FAILED ( core->d3d->device->CreateBuffer ( &descBufferLine, &subResourceDateLine, &vertexBufferLine ) ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of line vertex buffer failed!" );
    }

    allocated = true;
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"The game resources is successfully allocated." );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


const bool& GameWrapper::isReady ( void )
{
  return initialized;
};


void GameWrapper::render ( void )
{
  try
  {

    core->d3d->clearBuffers (); // note 

    if (core->d2d && core->debug)
      core->d2d->debugInfos (); // -- fps on screen representation

    // set vertex buffer (binds an array of vertex buffers to input-assembler stage)
    // basically, which vertices to read from when rendering
    unsigned int strides = sizeof ( Vertex );
    unsigned int offset = 0;
    // fourth parameter: constant array of stride values (one stride for each buffer in the vertex-buffer array)
    // fifth parameter: number of bytes between the first element and the element to use (usually zero)
    core->d3d->device->IASetVertexBuffers ( 0, 1, vertexBufferTriangle.GetAddressOf (), &strides, &offset );

    // set primitive topology (Direct3D has no idea about the mathematical conventions to use)
    // basically how to render the resource (vertex data) to screen
    core->d3d->device->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // draw 9 vertices, starting from vertex 0
    core->d3d->device->Draw ( 9, 0 );

    core->d3d->device->IASetVertexBuffers ( 0, 1, vertexBufferLine.GetAddressOf (), &strides, &offset );
    core->d3d->device->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_LINELIST );
    core->d3d->device->Draw ( 2, 0 );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void GameWrapper::update ( void )
{
  try
  {

    // update line vertex buffer
    HRESULT hResult;
    char modeX_1 { 0 };
    char modeY_1 { 0 };
    char modeX_2 { 0 };
    char modeY_2 { 0 };

    // map the data back to system memory using a sub-resource
    // second parameter: what CPU does when GPU is busy
    // note that in Direct3D11 a resource may contain sub-resources (additional parameters of device context method)
    // after the resource is mapped, any change to it is reflected to the vertex buffer.
    hResult = vertexBufferLine->Map ( D3D10_MAP_WRITE_NO_OVERWRITE, 0, &mappedLine.pData );
    if (FAILED ( hResult ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Mapping the resource data failed!" );
    }

    // update the sub-resource:

    // turn the line clockwise
    Vertex* vertexData = reinterpret_cast<Vertex*>(mappedLine.pData);
    //{ -0.2f, 0.0f, 0.0f, 0.13f, 0.13f, 0.13f },
    //{ 0.2f, 0.0f, 0.0f, 0.93f, 0.93f, 0.93f },

    static float temp { vertexData->x };

    if (vertexData->x < (vertexData + 1)->x)
    {
      modeX_1 = 1 ;
      modeY_1 = 1 ;
      modeX_2 = -1 ;
      modeY_2 = -1 ;
    } else
      if (vertexData->x > ( vertexData + 1 )->x)
      {
        modeX_1 = 1;
        modeY_1 = -1;
        modeX_2 = -1;
        modeY_2 = 1;
      }

    if ((vertexData + 1)->x < temp)
    {
      temp = vertexData->x;
      vertexData->x = (vertexData + 1)->x;
      (vertexData + 1)->x = temp;
      temp = vertexData->y;
      vertexData->y = (vertexData + 1)->y;
      (vertexData + 1)->y = temp;
      temp = vertexData->x ;
    }

    vertexData->x += modeX_1 * 0.0001f;
    vertexData->y += modeY_1 * 0.0001f;
    (vertexData + 1)->x += modeX_2 * 0.0001f;
    (vertexData + 1)->y += modeY_2 * 0.0001f;

    // randomize the vertex colours
    float rnd_1 { 0.0f };
    float rnd_2 { 0.0f };
    float rnd_3 { 0.0f };
    rnd_1 = ((rand () % 100) / static_cast<float>(100));
    rnd_2 = ((rand () % 100) / static_cast<float>(100));
    rnd_3 = ((rand () % 100) / static_cast<float>(100));
    vertexData->r = (vertexData + 1)->r = rnd_1;
    vertexData->g = (vertexData + 1)->g = rnd_2;
    vertexData->b = rnd_1 = (vertexData + 1)->b = rnd_3;

    // validates the pointer of the vertex buffer's resource and enables the GPU's read access upon.
    vertexBufferLine->Unmap ();

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


const bool GameWrapper::run ( void )
{
  try
  {

    MSG msg { 0 }; // a new message structure
    unsigned short counter { 0 };

    core->timer->event ( "reset" ); // reset (start)



    // main part (game engine)
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // Todo: research for a robust game loop:
    // mathematical simulation of time and reality, physics, multithreading
    //
    do // continuous loop
    {

#pragma region peekLoop
      if ((counter % 10) == 0)
      {
        // check and peek (get) window messages already placed in an event queue.
        // note the difference between the below two functions:
        // the get function, once called, actually waits for a message,
        // while peek function allows the normal flow, if there is no message in the message queue.
        while ( // peek loop for messages (empting the message queue)
                PeekMessage (
                  // pointer to a message structure to receive message information
                  &msg,
                  // handle to the window, whose messages is intended,
                  // and NULL as argument allow the retrieve of all messages for any window, which belongs to the current thread.
                  core->appHandle,
                  // to retrieve messages filtered through the introduced range. (both zero retunes all available messages)
                  // first message to last message
                  0, 0,
                  // removal flags specify how the messages are to be handled:
                  // additionally to below introduced argument, PM_NOREMOVE prevents the removal of messages in the queue,
                  // therefore after it is passed, the get function is additionally needed to actually retrieve the messages.
                  PM_REMOVE ))
          //while ( GetMessage ( &msg, NULL, 0, 0 ) ) // not a good one for a game, which needs to deliver 30 F/S
        {
          // translation of the virtual-key messages into character messages
          TranslateMessage ( &msg );
          // dispatching the message to the window procedure function, so the event could be handled appropriately.
          DispatchMessage ( &msg );
          // the evaluated value: exit (using intellisense or MSDN, the possible and obvious messages could be seen)
        }
        counter = 0;
      }
#pragma endregion



      // tick the timer to calculate a frame
      core->timer->tick ();
      // -- fps calculation
      core->frameStatistics ();

      if (!core->paused)
      {

        // -----------------------------------------------------------------------------------------------------------
        // a game loop purpose:
        // -- process inputted data


        // -----------------------------------------------------------------------------------------------------------
        // -- update the game universe/logic
        // Note the needed delta time
        render ();

        update ();

        // -----------------------------------------------------------------------------------------------------------
        // -- output a frame
        core->d3d->present ();

      } else
      {
        // Todo add additional suitable procedure can be done in paused state

        // if there is no suitable processing for paused state, set the engine at hibernation:
        std::this_thread::sleep_for ( std::chrono::milliseconds ( 100 ) );
      }

      counter++;
    } while (running == true);
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



    return true;
  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
    return false;
  }
};


void GameWrapper::shutdown ( void )
{
  try
  {

    initialized = false;
    unsigned long refCounts { 0 };
    //HRESULT hResult;
    refCounts = vertexBufferLine.Reset ();
    refCounts = vertexBufferTriangle.Reset ();
    if (core)
    {
      core->shutdown ();
      delete core;
    }

    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"The Game is successfully shut down." );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};
