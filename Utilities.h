// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,22.07.2019</created>
/// <changed>ʆϒʅ,01.08.2019</changed>
// ********************************************************************************

#ifndef UTILITIES_H
#define UTILITIES_H


// special expansion to exception container
class theException : public std::exception
{
private:
  const char* expected;
public:
  theException ();
  void set ( const char* );
  const char* what () const throw( );
};


// log severity
enum logType { info = 0, debug, warning, error };


// log container structure
struct Log
{
  unsigned int id;
  logType type { info };
  std::wstring cMoment;
  std::thread::id threadId;
  std::wstring threadName;
  std::wstring message;
};


// file stream policy
class toFile
{
private:
  std::ofstream fileStream;
  bool ready;
public:
  toFile ();
  ~toFile ();
  const bool& state ();
  bool write ( const Log& );
};


// Todo screen stream policy
//class toScreen {};


// logging engine
template<class tType>
class Logger
{
private:
  Log logEntity;
  static unsigned int counter;
  std::list<Log> buffer; // buffer list container
  tType policy; // output stream policy
  std::timed_mutex writeGuard; // write guard
  std::thread commit; // write engine thread
  // lock-free atomic flag (checking the running state) (standard initialization):
  std::atomic_flag operating { ATOMIC_FLAG_INIT };
public:
  Logger ();
  ~Logger ();
  void push ( const logType&,
              const std::thread::id&,
              const std::wstring&,
              const std::wstring& );

  template<class tType>
  friend void loggerEngine ( Logger<tType>* ); // write engine
};


// don't call this function: solution for linker error, when using templates.
void problemSolver ();


// configurations container
struct ConfigsContainer
{
  unsigned int Width;
  unsigned int Height;
  bool fullscreen;
};


// application configurations class
class Configurations
{
private:
  std::wstring pathToMyDocuments;
  std::wstring pathToSettings;
  bool valid;
  ConfigsContainer defaults;
  ConfigsContainer currents;
public:
  Configurations ();
  const bool& isValid ();
  const ConfigsContainer& getDefaults ( void );
  const ConfigsContainer& getSettings ( void );
  void apply ();
  void apply ( const ConfigsContainer& );
};// application configurations container


// string converter class
class Converter
{
private:
public:
  // standard string types converters (Lua needs)
  // Windows 10 builds upon Unicode natively, thus using wide strings is wise.
  static std::wstring strConverter ( const std::string& );
  static std::string strConverter ( const std::wstring& );
};


#endif // !UTILITIES_H
