// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,22.07.2019</created>
/// <changed>ʆϒʅ,23.07.2019</changed>
// ********************************************************************************

#ifndef LOGGING_H
#define LOGGING_H


// log events warnings messages errors
// buffer and write it to file
// consider safety


enum logType { info = 0, debug, warning, error };


struct Log
{
  unsigned short id;
  logType type;
  std::string cMoment;
  std::thread::id threadId;
  std::string threadName;
  std::string message;

  Log ();
  void set ( const logType&,
             const std::thread::id&,
             const std::string&,
             const std::string& );

  static unsigned short count;
};


class toFile
{
private:
  std::ofstream fileStream;
  bool ready;
public:
  toFile ();
  ~toFile ();
  void write ( std::list<Log>& );
};


//class toScreen {};


template<class tType>
class Logger
{
private:
  std::list<Log> buffer;
  tType policy;
  //toFile policy;
public:
  Logger ( const Log& );
  ~Logger ();
};


// don't call this function: solution for linker error, when using templates.
void temp ();


#endif // !LOGGING_H
