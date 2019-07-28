// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,22.07.2019</created>
/// <changed>ʆϒʅ,28.07.2019</changed>
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


Log::Log () : id ( 0 ), count ( 1 )
{
  type = logType::info;
  threadId = std::this_thread::get_id ();
  threadName = L"null";
  message = L"null";
  cMoment = L"null";
};


void Log::set ( const logType& t,
                const std::thread::id& tId,
                const std::wstring& tName,
                const std::wstring& msg )
{
  id = count;
  count++;
  type = t;
  threadId = tId;
  threadName = tName;
  message = msg;
  std::wstringstream current;
  SYSTEMTIME cDateT;
  GetLocalTime ( &cDateT );
  // date and time format: xx/xx/xx xx:xx:xx
  current << cDateT.wDay << '/' << cDateT.wMonth << '/' << cDateT.wYear << ' '
    << cDateT.wHour << ':' << cDateT.wMinute << ':' << cDateT.wSecond;
  cMoment = current.str ();
};


toFile::toFile () : ready ( false )
{
  try
  {
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
      MessageBoxA ( win->gethHandle (), "The log file could not be opened!", "Error", MB_OK | MB_ICONERROR );
      aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", L"The log file could not be opened!" );
    } else
    {
      MessageBoxA ( win->gethHandle (), ex.what (), "Error", MB_OK | MB_ICONERROR );
      aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", settings.strConverter ( ex.what () ) );
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
    std::wstringstream line;
    if ( ( running == false ) && ( gameState == L"shutting down" ) )
    {
      line << "\r\n\n";
      gameState = L"uninitialized";
    } else
      line << "\r\n";

    line << entity.id << "\t\t" << entity.cMoment << '\t';
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

    if ( ( running == true ) && ( gameState == L"initialized" ) )
    {
      line << '\n';
      gameState = L"gaming";
    }

    if ( ready )
      fileStream << settings.strConverter ( line.str () );
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
      MessageBoxA ( win->gethHandle (), "File output stream was not ready!", "Error", MB_OK | MB_ICONERROR );
      aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", L"File output stream was not ready!" );
    } else
    {
      MessageBoxA ( win->gethHandle (), ex.what (), "Error", MB_OK | MB_ICONERROR );
      aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", settings.strConverter ( ex.what () ) );
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
  aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The logging engine is shutting down..." );
  logEngineToFile.push ( aLog );
  std::this_thread::sleep_for ( std::chrono::milliseconds { 100 } );
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
    aLog.set ( logType::info, std::this_thread::get_id (), L"logThread", L"Logging engine is started:\n\nFull-featured surveillance is the utter most goal in a digital world, and frankly put, it is well justified! ^,^\n" );
    logEngineToFile.push ( aLog );

    // initializing and not locking the mutex object (mark as not owing a lock)
    std::unique_lock<std::timed_mutex> lock ( engine->writeGuard, std::defer_lock );

    do
    {
      if ( engine->buffer.size () )
      {
        if ( !lock.try_lock_for ( std::chrono::milliseconds { 30 } ) )
          continue;
        for ( auto& element : engine->buffer )
          //engine->policy.write ( element );
          if ( !engine->policy.write ( element ) )
          {
            aLog.set ( logType::warning, std::this_thread::get_id (), L"logThread", L"Dumping wasn't possible." );
            logEngineToFile.push ( aLog );
          }
        engine->buffer.clear ();
        lock.unlock ();
      }
    } while ( engine->operating.test_and_set () || engine->buffer.size () );
  }
  catch ( const std::exception& ex )
  {
    MessageBoxA ( win->gethHandle (), ex.what (), "Error", MB_OK | MB_ICONERROR );
    aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", settings.strConverter ( ex.what () ) );
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


Configuration::Configuration ()
{
  Width = 640;
  Height = 480;
};


const Configuration& Configuration::set ( void )
{
  return *this;
};


Configure::Configure ()
{
  try
  {
    PWSTR docPath { NULL };

    HRESULT hResult = SHGetKnownFolderPath ( FOLDERID_Documents, NULL, NULL, &docPath );
    if ( FAILED ( hResult ) )
    {
      MessageBoxA ( win->gethHandle (), "The path to document directory is unknown!", "Error", MB_OK | MB_ICONERROR );
      aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", L"The path to document directory is unknown!" );
      logEngineToFile.push ( aLog );
    }

    pathToMyDocuments = docPath;
    //std::wstring path { pathToMyDocuments + L"\\settings.lua" };
    std::wstring path { L"C:\\Users\\Mehrdad\\Source\\Repos\\LearningDirectX\\settings.lua" };

    sol::state configs;
    // opening the configuration file
    configs.script_file ( strConverter ( path ) );
    // read or use the application defaults:
    current.Width = configs ["configurations"]["resolution"]["width"].get_or ( defaults.Width );
    // the sol state class is constructed like a table, thus nested variables are accessible like multidimensional arrays.
    current.Height = configs ["configurations"]["resolution"]["height"].get_or ( defaults.Height );
    if ( current.Height != defaults.Height )
    {
      valid = true;
      aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The configuration file is successfully read." );
    } else
    {
      valid = false;
      aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", L"Non-existent or invalid configuration file. Default settings are used!" );
    }
    logEngineToFile.push ( aLog );
  }
  catch ( const std::exception& ex )
  {
    MessageBoxA ( win->gethHandle (), ex.what (), "Error", MB_OK | MB_ICONERROR );
    aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", settings.strConverter ( ex.what () ) );
    logEngineToFile.push ( aLog );
  }
};


Configure::~Configure ()
{

};


void Configure::set ( const Configuration& )
{
  // Todo new settings to configuration file
};


const Configuration& Configure::set ( void )
{
  return current;
};


std::wstring Configure::strConverter ( const std::string& str )
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> entity;
  return entity.from_bytes ( str );
};


std::string Configure::strConverter ( const std::wstring& wstr )
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> entity;
  return entity.to_bytes ( wstr );
};
