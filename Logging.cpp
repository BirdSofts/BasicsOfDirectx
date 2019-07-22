// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,22.07.2019</created>
/// <changed>ʆϒʅ,23.07.2019</changed>
// ********************************************************************************

#include "LearningDirectX.h"
#include "Logging.h"


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
  current << cDateT.wDay << '/' << cDateT.wMonth << '/' << cDateT.wYear << " - "
    << cDateT.wHour << ':' << cDateT.wMinute << ':' << cDateT.wSecond;
  cMoment = current.str ();
};


unsigned short Log::count { 0 };


toFile::toFile ()
{
  try
  {
    ready = false;
    fileStream.open ( "filename.log", std::ofstream::binary );
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
      MessageBox ( 0, L"The log file could not be opened for output operation!", L"Error", MB_OK | MB_ICONERROR );
    else
      MessageBox ( 0, LPCWCHAR ( ex.what () ), L"Error", MB_OK | MB_ICONERROR );
  }
};


toFile::~toFile ()
{
  fileStream.close ();
  ready = false;
};


void toFile::write ( std::list<Log>& content )
{
  try
  {
    // Todo add write safety in thread
    std::stringstream line;
    while ( content.size () )
    {
      std::stringstream line;
      line << content.front ().id << '\t' << content.front ().cMoment << '\t';
      switch ( content.front ().type )
      {
        case 0:
          line << "INFO:\t";
          break;
        case 1:
          line << "DEBUG:\t";
          break;
        case 2:
          line << "WARNING:\t";
          break;
        case 3:
          line << "ERROR:\t";
          break;
      }
      line << content.front ().threadId << '\t' << content.front ().threadName << '\t' << content.front ().message << '\n';
      if ( ready )
      {
        fileStream << line.str ();
        content.pop_front ();
      } else
      {
        anException.set ( "logW" );
        throw anException;
      }
    }
    fileStream.flush ();

  }
  catch ( const std::exception& ex )
  {
    if ( ex.what () == "logW" )
      MessageBox ( 0, L"File output stream was not ready!", L"Error", MB_OK | MB_ICONERROR );
    else
      MessageBox ( 0, LPCWCHAR ( ex.what () ), L"Error", MB_OK | MB_ICONERROR );
  }
};


template<class tType>
Logger<tType>::Logger ( const Log& entity ) : policy ()
{
  for ( char i = 0; i < 10; i++ )
  {
    buffer.push_back ( entity );
  }
  policy.write ( buffer );
};


template<class tType>
Logger<tType>::~Logger ()
{};


// don't call this function: solution for linker error, when using templates.
void temp ()
{
  Logger<toFile> tempObj(aLog);
}
