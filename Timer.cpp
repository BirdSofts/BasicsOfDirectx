// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,28.07.2019</created>
/// <changed>ʆϒʅ,30.07.2019</changed>
// ********************************************************************************

#include "LearningDirectX.h"
#include "Timer.h"


Timer::Timer () : timeStart ( 0 ), timeCurrent ( 0 ), timePrevious ( 0 ),
timeStopped ( 0 ), timeTotalIdle ( 0 ),
secondsPerCount ( 0.0 ), deltaTime ( 0.0 ), stopped ( false )
{
  try
  {
    long long frequency { 0 };
    // Windows high performance timer: 'QueryPerformanceFrequency' 'QueryPerformanceCounter':
    // note that, if the system doesn't support it, C++ 11 standard chrono is the replacement.
    // -- QueryPerformanceFrequency:
    // current frequency of the performance counter (the counts per second of the performance timer),
    // which is a fixed value at system boot and consistent across all processors.
    // -- QueryPerformanceCounter:
    // current value of the performance counter in counts,
    // consisted of a high-precision (<1μs) time stamp, usable in time-interval measurements.
    // note that the function returns zero if an error is occurred.
    // Todo implement C++ standard chrono
    if ( QueryPerformanceFrequency ( ( LARGE_INTEGER*) & frequency ) )
    {
      // once calculated seconds per count (reciprocal of the frequency)
      // TimeValueInSeconds = TimeValueInCounts / Frequency
      secondsPerCount = 1.0 / ( double) frequency;

#ifndef _NOT_DEBUGGING
      aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The high-precision timer is instantiated." );
      logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

    } else
    {
      throw;
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


Timer::~Timer ()
{

#ifndef _NOT_DEBUGGING
  aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The timer is successfully destructed." );
  logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

};


const double Timer::getTotal ()
{
  // a total from the start of the game
  double temp;
  if ( stopped )
    temp = ( timeStopped - timeStart - timeTotalIdle ) * secondsPerCount;
  else
    temp = ( timeCurrent - timeStart - timeTotalIdle ) * secondsPerCount;
  return temp;
};


const double& Timer::getDelta ()
{
  return deltaTime;
};


void Timer::event ( const char* type )
{
  long long current;
  try
  {
    if ( QueryPerformanceCounter ( ( LARGE_INTEGER*) & current ) )
    {
      // if start is requested as event
      if ( ( type == "start" ) && ( stopped ) )
      {
        timeTotalIdle += ( current - timeStopped ); // total ideal time up until now
        timePrevious = current; // sync to the current time
        // make ready for next stop:
        timeStopped = 0;
        stopped = false;

#ifndef _NOT_DEBUGGING
        aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The timer is started." );
        logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

      }

      // if stop is requested as event
      if ( ( type == "stop" ) && ( !stopped ) )
      {
        timeStopped = current; // the start time of the stop
        stopped = true;

#ifndef _NOT_DEBUGGING
        aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The timer is stopped." );
        logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

      }

      // if reset is requested as event
      if ( type == "reset" )
      {
        timeStart = current;
        timePrevious = current;
        timeStopped = 0;
        stopped = false;

#ifndef _NOT_DEBUGGING
        aLog.set ( logType::info, std::this_thread::get_id (), L"mainThread", L"The timer is reset." );
        logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

      }

    } else
      throw;
  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    aLog.set ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
    logEngineToFile.push ( aLog );
#endif // !_NOT_DEBUGGING

  }
};


void Timer::tick ()
{
  // tick and calculate the time between two frames
  try
  {
    if ( stopped )
      deltaTime = 0.0; // the elapsed time in a stopped state
    else
      if ( QueryPerformanceCounter ( ( LARGE_INTEGER*) & timeCurrent ) )
      {
        deltaTime = ( timeCurrent - timePrevious ) * secondsPerCount; // the elapsed time of one frame
        timePrevious = timeCurrent; // save for next tick

        // in case, a negative delta means that the processor goes idle. the cause can be an overflow,
        // a power save mode or the movement of the process to another processor.
        if ( deltaTime < 0.0 )
          deltaTime = 0.0;
      } else
      {
        throw;
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
