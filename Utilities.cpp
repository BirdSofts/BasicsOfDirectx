// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,22.07.2019</created>
/// <changed>ʆϒʅ,24.07.2019</changed>
// ********************************************************************************

#include "LearningDirectX.h"
#include "Utilities.h"


theException::theException () : expected ( "null" ) {};
void theException::set ( const char* prm )
{
  expected = prm;
};
const char* theException::what () const throw( )
{
  return expected;
};


Log::Log ()
{
  id = 0;
  count++;
  type = logType::info;
  threadId = std::this_thread::get_id ();
  threadName = "null";
  message = "null";
  cMoment = "null";
};


void Log::set ( const logType& t,
                const std::thread::id& tId,
                const std::string& tName,
                const std::string& msg )
{
  id = count;
  count++;
  type = t;
  threadId = tId;
  threadName = tName;
  message = msg;
  std::stringstream current;
  SYSTEMTIME cDateT;
  GetLocalTime ( &cDateT );
  // date and time format: xx/xx/xx xx:xx:xx
  current << cDateT.wDay << '/' << cDateT.wMonth << '/' << cDateT.wYear << " "
    << cDateT.wHour << ':' << cDateT.wMinute << ':' << cDateT.wSecond;
  cMoment = current.str ();
};


unsigned short Log::count { 0 };


toFile::toFile ()
{
  try
  {
    ready = false;
    fileStream.open ( "dump.log", std::ofstream::binary );
    if ( fileStream.is_open () )
      ready = true;
    else
    {
      anException.set ( "fileO" );
      throw anException;
    }
  }
  catch ( const std::exception& ex )
  {
    if ( ex.what () == "fileO" )
    {
      MessageBoxA ( 0, "The log file could not be opened for output operation!", "Error", MB_OK | MB_ICONERROR );
      aLog.set ( logType::error, std::this_thread::get_id (), "mainThread", "The log file could not be opened for output operation!" );
    } else
    {
      MessageBoxA ( 0, ex.what (), "Error", MB_OK | MB_ICONERROR );
      aLog.set ( logType::error, std::this_thread::get_id (), "mainThread", ex.what () );
    }
    logEngineToFile.push ( aLog );
  }
};


toFile::~toFile ()
{
  ready = false;
  fileStream.close ();
};


const bool& toFile::state ()
{
  return ready;
}


bool toFile::write ( const Log& entity )
{
  try
  {
    std::stringstream line;
    line << "\r\n" << entity.id << "\t\t" << entity.cMoment << '\t';
    switch ( entity.type )
    {
      case 0:
        line << "INFO:    ";
        break;
      case 1:
        line << "DEBUG:   ";
        break;
      case 2:
        line << "WARNING: ";
        break;
      case 3:
        line << "ERROR:   ";
        break;
    }
    line << entity.threadId << '\t' << entity.threadName << '\t' << entity.message;
    if ( ready )
      fileStream << line.str ();
    else
    {
      anException.set ( "logW" );
      throw anException;
    }
    return true;
  }
  catch ( const std::exception& ex )
  {
    if ( ex.what () == "logW" )
    {
      MessageBoxA ( 0, "File output stream was not ready!", "Error", MB_OK | MB_ICONERROR );
      aLog.set ( logType::error, std::this_thread::get_id (), "mainThread", "File output stream was not ready!" );
    } else
    {
      MessageBoxA ( 0, ex.what (), "Error", MB_OK | MB_ICONERROR );
      aLog.set ( logType::error, std::this_thread::get_id (), "mainThread", ex.what () );
    }
    logEngineToFile.push ( aLog );
    return false;
  }
};

template<class tType>
void loggerEngine ( Logger<tType>* engine );
template<class tType>
Logger<tType>::Logger () : policy (), writeGuard ()
{
  //operating = ATOMIC_FLAG_INIT; // standard initialization
  if ( policy.state () )
  {
    operating.test_and_set (); // mark the write engine as running
    commit = std::move ( std::thread { loggerEngine<tType>, this } );
  }
};


template<class tType>
Logger<tType>::~Logger ()
{
  operating.clear ();
  commit.join ();
  buffer.clear ();
};


template<class tType>
void Logger<tType>::push ( const Log& entity )
{
  std::lock_guard<std::timed_mutex> lock ( writeGuard );
  buffer.push_back ( entity );
};


template<class tType>
void loggerEngine ( Logger<tType>* engine )
{
  try
  {
    // dump engine: write the present logs' data
    std::this_thread::sleep_for ( std::chrono::milliseconds { 20 } );
    aLog.set ( logType::info, std::this_thread::get_id (), "logThread", "Logging engine is started: Full-featured surveillance is the utter most goal in a digital world, and frankly put, it is well justified! ^,^" );
    logEngineToFile.push ( aLog );

    // initializing and not locking the mutex object (mark as not owing a lock)
    std::unique_lock<std::timed_mutex> lock ( engine->writeGuard, std::defer_lock );

    do
    {
      std::this_thread::sleep_for ( std::chrono::milliseconds { 50 } );
      if ( engine->buffer.size () )
      {
        if ( !lock.try_lock_for ( std::chrono::milliseconds { 50 } ) )
          continue;
        for ( auto& element : engine->buffer )
          engine->policy.write ( element );
        //if (!engine->policy.write ( element ))

        engine->buffer.clear ();
        lock.unlock ();
      }
    } while ( engine->operating.test_and_set () || engine->buffer.size () );
  }
  catch ( const std::exception& ex )
  {
    MessageBoxA ( 0, ex.what (), "Error", MB_OK | MB_ICONERROR );
    aLog.set ( logType::error, std::this_thread::get_id (), "mainThread", ex.what () );
    logEngineToFile.push ( aLog );
  }
};


// don't call this function: solution for linker error, when using templates.
void problemSolver ()
{
  Logger<toFile> tempObj;
  tempObj.push ( aLog );
  //loggerEngine ( &tempObj );
}
