// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,04.08.2019</created>
/// <changed>ʆϒʅ,05.08.2019</changed>
// ********************************************************************************

#include "D2D.h"
#include "Shared.h"


Direct2D::Direct2D ( TheCore* coreObject ) : theCore ( coreObject ),
initialized ( false )
{
  try
  {



    initialized = true;

#ifndef _NOT_DEBUGGING
    //PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Direct2D is successfully initialized." );
#endif // !_NOT_DEBUGGING

  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};


const bool& Direct2D::isInitialized ()
{
  return initialized;
};


void Direct2D::shutdown ( void )
{
  try
  {
    if ( theCore )
    {
      theCore = nullptr;
      delete theCore;
    }

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"Direct2D is successfully destructed." );
#endif // !_NOT_DEBUGGING

  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};
