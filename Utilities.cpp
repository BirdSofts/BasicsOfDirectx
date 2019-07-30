// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,22.07.2019</created>
/// <changed>ʆϒʅ,30.07.2019</changed>
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

#ifndef _NOT_DEBUGGING
    if ( ex.what () == "fileO" )
    {
      aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", L"The log file could not be opened!" );
    } else
    {
      aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
    }
    logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

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
      fileStream << Converter::strConverter ( line.str () );
    else
    {
      anException.set ( "logW" );
      throw anException;
    }
    return true;
  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    if ( ex.what () == "logW" )
    {
      aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", L"File output stream was not ready!" );
    } else
    {
      aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
    }
    logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

    return false;
  }
};

template<class tType>
void loggerEngine ( Logger<tType>* engine );
template<class tType>
Logger<tType>::Logger () : policy (), writeGuard ()
{
  if ( policy.state () )
  {
    operating.test_and_set (); // mark the write engine as running
    commit = std::move ( std::thread { loggerEngine<tType>, this } );
  }
};


template<class tType>
Logger<tType>::~Logger ()
{

#ifndef _NOT_DEBUGGING
  aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The logging engine is shutting down..." );
  logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

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
#ifndef _NOT_DEBUGGING
    aLog.set ( logType::info, std::this_thread::get_id (), L"logThread", L"Logging engine is started:\n\nFull-featured surveillance is the utter most goal in a digital world, and frankly put, it is well justified! ^,^\n" );
    logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

    // initializing and not locking the mutex object (mark as not owing a lock)
    std::unique_lock<std::timed_mutex> lock ( engine->writeGuard, std::defer_lock );

    do
    {
      if ( engine->buffer.size () )
      {
        if ( !lock.try_lock_for ( std::chrono::milliseconds { 30 } ) )
          continue;
        for ( auto& element : engine->buffer )
          if ( !engine->policy.write ( element ) )
          {

#ifndef _NOT_DEBUGGING
            aLog.set ( logType::warning, std::this_thread::get_id (), L"logThread", L"Dumping wasn't possible." );
            logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

          }
        engine->buffer.clear ();
        lock.unlock ();
      }
    } while ( engine->operating.test_and_set () || engine->buffer.size () );
  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
    logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

  }
};


// don't call this function: solution for linker error, when using templates.
void problemSolver ()
{
  Logger<toFile> tempObj;
  tempObj.push ( aLog );
}


Configurations::Configurations ()
{
  try
  {
    valid = false;

    // defaults initialization:
    defaults.Width = 640;
    defaults.Height = 480;
    defaults.fullscreen = false;

    // currents initialization:
    currents.Width = 0;
    currents.Height = 0;
    currents.fullscreen = false;

    PWSTR docPath { NULL };
    HRESULT hResult = SHGetKnownFolderPath ( FOLDERID_Documents, NULL, NULL, &docPath );
    if ( FAILED ( hResult ) )
    {
      MessageBoxA ( NULL, "The path to document directory is unknown!", "Error", MB_OK | MB_ICONERROR );

#ifndef _NOT_DEBUGGING
      aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", L"The path to document directory is unknown!" );
      logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

      pathToMyDocuments = L"";
    } else
      pathToMyDocuments = docPath;

    std::wstring path { L"" };

    if ( pathToMyDocuments == L"" )
    {
      path = L"C:\\TheGame";
      hResult = SHCreateDirectory ( NULL, path.c_str () );
      if ( FAILED ( hResult ) )
        path = L"C:\\settings.lua";
      else
        path += L"\\settings.lua";
    } else
      std::wstring path { pathToMyDocuments + L"\\settings.lua" };


    // development time path
    path = { L"C:\\Users\\Mehrdad\\Source\\Repos\\LearningDirectX\\settings.lua" };


    pathToSettings = path;
    // Lua accepts a string type as path
    std::string pathStr { "" };
    pathStr = Converter::strConverter ( pathToSettings );

    for ( char i = 0; i < 2; i++ )
    {
      // read the configuration
      sol::state configs;
      try
      {
        configs.safe_script_file ( pathStr );
        // opening the configuration file
        // read or use the application defaults:
        currents.Width = configs ["configurations"]["resolution"]["width"].get_or ( defaults.Width );
        // the sol state class is constructed like a table, thus nested variables are accessible like multidimensional arrays.
        currents.Height = configs ["configurations"]["resolution"]["height"].get_or ( defaults.Height );
        currents.fullscreen = configs ["fullscreen"].get_or ( defaults.fullscreen );
      }
      catch ( const std::exception& ex )
      {

#ifndef _NOT_DEBUGGING
        aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
        logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

      }

      // validation
      if ( ( currents.Width != 0 ) && ( currents.Height != 0 ) && ( !valid ) )
      {
        valid = true;

#ifndef _NOT_DEBUGGING
        aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread",
                   L"The configuration file is successfully read:" );
        logEngineToFile.push ( aLog );
        aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread",
                   L"Resolution: (" + std::to_wstring ( currents.Width ) + L" x "
                   + std::to_wstring ( currents.Height ) + L" )" );
        logEngineToFile.push ( aLog );
        aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread",
                   L"fullscreen: " + std::to_wstring ( currents.fullscreen ) );
        logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

        break;
      } else
      {

#ifndef _NOT_DEBUGGING
        aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", L"Non-existent or invalid configuration file!" );
        logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

        // rewrite the configuration file with defaults
        std::ofstream writeStream ( path.c_str () );
        if ( writeStream.good () )
        {
          std::stringstream settingsLine;
          settingsLine << "configurations =\n\t{\n" <<
            "\t\tresolution = { width = " << std::to_string ( defaults.Width ) <<
            " , height = " << std::to_string ( defaults.Height ) << " },\n" <<
            "\t\tfullscreen = " << std::to_string ( defaults.fullscreen ) << "\n\t}";
          writeStream << settingsLine.str ();
          writeStream.close ();

#ifndef _NOT_DEBUGGING
          aLog.set ( logType::warning, std::this_thread::get_id (), L"mainThread", L"Configuration file is now rewritten with default settings." );
          logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

        }
      }
    }
  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
    logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

  }
};


const bool& Configurations::isValid ()
{
  return valid;
};


const ConfigsContainer& Configurations::getDefaults ( void )
{
  return currents;
};


const ConfigsContainer& Configurations::get ( void )
{
  return currents;
};


void Configurations::apply ()
{
  //xx Todo new settings to configuration file
  // Todo add user settings
  std::ofstream writeStream ( pathToSettings.c_str () );
  if ( writeStream.good () )
  {
    std::stringstream settingsLine;
    settingsLine << "configurations =\n\t{\n" <<
      "\t\tresolution = { width = 800 , height = 600 },\n" <<
      "\t\tfullscreen = 0\n\t}";
    writeStream << settingsLine.str ();
    writeStream.close ();
  }

#ifndef _NOT_DEBUGGING
  aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread",
             L"The configuration file is successfully written:" );
  logEngineToFile.push ( aLog );
  aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread",
             L"Resolution: (" + std::to_wstring ( currents.Width ) + L" x "
             + std::to_wstring ( currents.Height ) + L" )" );
  logEngineToFile.push ( aLog );
  aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread",
             L"fullscreen: " + std::to_wstring ( currents.fullscreen ) );
  logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

};


void Configurations::apply ( const ConfigsContainer& )
{

};


std::wstring Converter::strConverter ( const std::string& str )
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> entity;
  return entity.from_bytes ( str );
};


std::string Converter::strConverter ( const std::wstring& wstr )
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> entity;
  return entity.to_bytes ( wstr );
};
