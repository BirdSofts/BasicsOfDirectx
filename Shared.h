// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,30.07.2019</created>
/// <changed>ʆϒʅ,01.08.2019</changed>
// ********************************************************************************

#ifndef SHARED_H
#define SHARED_H


#include "Utilities.h"


//template<class tType>
  //static std::shared_ptr<tType> pointerEntity;
  //static tType* get (); // generic locator
  //static void provider ( std::shared_ptr<tType> ); // generic provider


class PointerProvider
{
private:
  static std::shared_ptr<theException> exception; // shared pointer to exception class
  static std::shared_ptr<Configurations> configuration; // shared pointer to configuration class
  static std::shared_ptr<Logger<toFile>> fileLogger; // shared pointer to file logger class
public:

  // smart shared pointers point to exceptions, provide extended lifetime and are passable across threads:
  static std::exception_ptr exceptionPointer; // smart shared pointer-like type

  static theException* getException (); // exception locator
  static Configurations* getConfiguration (); // configuration locator
  static Logger<toFile>* getFileLogger (); // file logger locator
  static void exceptionProvider ( std::shared_ptr<theException> ); // exception provider
  static void configurationProvider ( std::shared_ptr<Configurations> ); // configuration provider
  static void fileLoggerProvider ( std::shared_ptr<Logger<toFile>> ); // file logger provider
};


#endif // !SHARED_H
