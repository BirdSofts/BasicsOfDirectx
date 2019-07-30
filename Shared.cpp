// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,30.07.2019</created>
/// <changed>ʆϒʅ,31.07.2019</changed>
// ********************************************************************************

#include "LearningDirectX.h"
#include "Shared.h"
#include "Utilities.h"


//template<class tType>
//std::shared_ptr<tType> PointerProvider<tType>::pointerEntity = nullptr;
//template<class tType>
//tType* PointerProvider<tType>::get ()
//{
//  return pointerEntity.get ();
//};
//template<class tType>
//void PointerProvider<tType>::provider ( std::shared_ptr<tType> object )
//{
//  pointerEntity = object;
//};


std::shared_ptr<theException> PointerProvider::exception = nullptr;

std::shared_ptr<Configurations> PointerProvider::configuration = nullptr;

std::shared_ptr<Logger<toFile>> PointerProvider::fileLogger = nullptr;


theException* PointerProvider::getException ()
{
  return exception.get ();
};


Configurations* PointerProvider::getConfiguration ()
{
  return configuration.get ();
};


Logger<toFile>* PointerProvider::getFileLogger ()
{
  return fileLogger.get ();
};


void PointerProvider::exceptionProvider ( std::shared_ptr<theException> object )
{
  exception = object;
};


void PointerProvider::configurationProvider ( std::shared_ptr<Configurations> object )
{
  configuration = object;
};


void PointerProvider::fileLoggerProvider ( std::shared_ptr<Logger<toFile>> object )
{
  fileLogger = object;
};
