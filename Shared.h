// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,30.07.2019</created>
/// <changed>ʆϒʅ,31.07.2019</changed>
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
  static std::shared_ptr<theException> exception;
  static std::shared_ptr<Configurations> configuration;
  static std::shared_ptr<Logger<toFile>> fileLogger;
public:
  static theException* getException (); // exception locator
  static Configurations* getConfiguration (); // configuration locator
  static Logger<toFile>* getFileLogger (); // file logger locator
  static void exceptionProvider ( std::shared_ptr<theException> ); // exception provider
  static void configurationProvider ( std::shared_ptr<Configurations> ); // configuration provider
  static void fileLoggerProvider ( std::shared_ptr<Logger<toFile>> ); // file logger provider
};


#endif // !SHARED_H
