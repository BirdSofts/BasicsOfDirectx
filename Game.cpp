// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,24.07.2019</created>
/// <changed>ʆϒʅ,16.08.2019</changed>
// ********************************************************************************

#include "Game.h"
#include "Shared.h"


GameWrapper::GameWrapper ( HINSTANCE& h_instance ) : initialized ( false )
{
  try
  {

    // the game framework instantiation
    core = new (std::nothrow) TheCore ( h_instance, this );

    if (core->isInitialized ())
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"The framework is successfully initialized." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "inFc" );
      throw* PointerProvider::getException ();
    }

    initialized = true;

    allocateResources ();

  }
  catch (const std::exception& ex)
  {
    if (ex.what () == "crFc")
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Initialization of framework failed!" );
#endif // !_NOT_DEBUGGING

    } else
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING
    }
  }
};


void GameWrapper::allocateResources ( void )
{
  try
  {
    allocated = false;

    // a triangle
    Vertex triangle [] {
      {0.0f, 0.1f, 0.3f},
      {0.11f, -0.1f, 0.3f},
      {-0.11f, -0.1f, 0.3f}
    };

    // buffer description
    D3D10_BUFFER_DESC descBuffer;
    descBuffer.ByteWidth = sizeof ( Vertex ) * ARRAYSIZE ( triangle ); // buffer size
    descBuffer.Usage = D3D10_USAGE_DEFAULT; // default: only GPC can read and write
    descBuffer.BindFlags = D3D10_BIND_VERTEX_BUFFER; // how to bound to graphics pipeline
    descBuffer.CPUAccessFlags = 0; // CPU access
    //descBuffer.MiscFlags = D3D10_RESOURCE_MISC_GDI_COMPATIBLE;
    descBuffer.MiscFlags = 0; // for now
    //descBuffer.StructureByteStride = 0; // Direct3D 11: structured buffer (the size of each element)

    // data, with which the buffer is initialized
    D3D10_SUBRESOURCE_DATA subResourceDate = {
      triangle, // pointer to data in system memory (copy to GPU)
      0, // distance between the lines of the texture in bytes (not needed for vertex buffer)
      0 }; // distance between the depth levels in bytes (not needed for vertex buffer)

    // vertex buffer: purpose: maintain system and video memory
    // note E_OUTOFMEMORY: self-explanatory
    if (SUCCEEDED ( core->d3d->device->CreateBuffer ( &descBuffer, &subResourceDate, &vertexBuffer ) ))
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Vertex buffer is successfully created." );
#endif // !_NOT_DEBUGGING

    } else
    {
      PointerProvider::getException ()->set ( "crVb" );
      throw* PointerProvider::getException ();
    }

    allocated = true;

  }
  catch (const std::exception& ex)
  {
    if (ex.what () == "crVb")
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of vertex buffer failed!" );
#endif // !_NOT_DEBUGGING

    } else
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

    }
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
    core->d3d->clearBuffers ();

#ifndef _NOT_DEBUGGING
    core->d2d->debugInfos (); // -- fps on screen representation
#endif // !_NOT_DEBUGGING

    // set vertex buffer (binds an array of vertex buffers to input-assembler stage)
    // basically, which vertices to read from when rendering
    unsigned int strides = sizeof ( Vertex );
    unsigned int offset = 0;
    // fourth parameter: constant array of stride values (one stride for each buffer in the vertex-buffer array)
    // fifth parameter: number of bytes between the first element and the element to use (usually zero)
    core->d3d->device->IASetVertexBuffers ( 0, 1, vertexBuffer.GetAddressOf (), &strides, &offset );

    // set primitive topology (Direct3D has no idea about the mathematical conventions to use)
    // basically how to render the resource (vertex data) to screen
    core->d3d->device->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // draw 3 vertices, starting from vertex 0
    core->d3d->device->Draw ( 3, 0 );

  }
  catch (const std::exception& ex)
  {
    //    if (ex.what () == "")
    //    {
    //
    //#ifndef _NOT_DEBUGGING
    //      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
    //                                                L"!" );
    //#endif // !_NOT_DEBUGGING
    //
    //    } else
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

    }
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


      // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      // Todo: research for a robust game loop:
      // mathematical simulation of time and reality, physics, multithreading
      //

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

        // -----------------------------------------------------------------------------------------------------------
        // -- output a frame
        core->d3d->present ();

        // rebind: the process is needed after each call to present, since in flip and discard mode the view targets are released.
        core->d3d->device->OMSetRenderTargets ( 1, core->d3d->rtView.GetAddressOf (), core->d3d->dsView.Get () );

      } else
      {
        // Todo add additional suitable procedure can be done in paused state

        // if there is no suitable processing for paused state, set the engine at hibernation:
        std::this_thread::sleep_for ( std::chrono::milliseconds ( 100 ) );
      }
      // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

      counter++;
    } while (running == true);

    return true;
  }
  catch (const std::exception& ex)
  {
    //    if (ex.what () == "")
    //    {
    //
    //#ifndef _NOT_DEBUGGING
    //      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
    //                                                L"" );
    //#endif // !_NOT_DEBUGGING
    //
    //    } else
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

    }
  }
  return false;
};


void GameWrapper::shutdown ( void )
{
  try
  {
    initialized = false;
    //unsigned long refCounts { 0 };
    //HRESULT hResult;
    vertexBuffer.Reset ();
    if (core)
    {
      core->shutdown ();
      delete core;
    }

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"The Game is successfully shut down." );
#endif // !_NOT_DEBUGGING

  }
  catch (const std::exception& ex)
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};
