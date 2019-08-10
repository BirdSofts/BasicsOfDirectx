// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,28.07.2019</created>
/// <changed>ʆϒʅ,10.08.2019</changed>
// ********************************************************************************

#include "Timer.h"
#include "Shared.h"


Timer::Timer () : initialized ( false ), timeStart ( 0 ), timeCurrentFrame ( 0 ), timePreviousFrame ( 0 ),
timeLastStopped ( 0 ), timeTotalIdle ( 0 ),
secondsPerCount ( 0.0 ), timeDelta ( 0.0 ), paused ( false )
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
    // the amounts of counts per seconds consisted of a high-precision (<1μs) time stamp,
    // usable in time-interval measurements.
    // note that the function returns zero if an error is occurred.
    // Todo implement C++ standard chrono
    if (QueryPerformanceFrequency ( ( LARGE_INTEGER*) & frequency ))
    {
      // once calculated seconds per count (reciprocal of the number of counts per seconds)
      // TimeValueInSeconds = ActualTimeValue / Frequency
      secondsPerCount = double ( 1 ) / frequency;

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"The high-precision timer is successfully instantiated." );
#endif // !_NOT_DEBUGGING

      initialized = true;
    } else
    {
      PointerProvider::getException ()->set ( "crT" );
      throw* PointerProvider::getException ();
      initialized = false;
    }
  }
  catch (const std::exception& ex)
  {

    if (ex.what () == "crT")

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"The high-precision timer instantiation failed!" );
#endif // !_NOT_DEBUGGING

    else

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};


const bool& Timer::isInitialized ()
{
  return initialized;
};


const double Timer::getTotal ()
{
  // total running time from the start of the game
  double temp;
  if (paused)
    temp = (timeLastStopped - timeStart - timeTotalIdle) * secondsPerCount;
  else
    temp = (timeCurrentFrame - timeStart - timeTotalIdle) * secondsPerCount;
  return temp;
};


const double& Timer::getDelta ()
{
  return timeDelta;
};


const bool& Timer::isPaused ()
{
  return paused;
};


void Timer::event ( const char* type )
{
  long long current;
  try
  {
    if (QueryPerformanceCounter ( ( LARGE_INTEGER*) & current ))
    {
      // if start is requested as event (invoked at game reactivation)
      if ((type == "start") && (paused))
      {
        timeTotalIdle += (current - timeLastStopped); // calculate total ideal
        timePreviousFrame = current; // prepare the calculation of this frame
        // make ready for next stop:
        timeLastStopped = 0;
        paused = false;

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                  L"The timer is successfully started." );
#endif // !_NOT_DEBUGGING

      }

      // if pause is requested as event (invoked at game deactivation)
      if ((type == "pause") && (!paused))
      {
        timeLastStopped = current; // store the time for later use
        paused = true;

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                  L"The timer is successfully stopped." );
#endif // !_NOT_DEBUGGING

      }

      // if reset is requested as event (start of the game loop)
      if (type == "reset")
      {
        // prepare the timer:
        timeStart = current;
        timePreviousFrame = current;
        timeLastStopped = 0;
        paused = false;

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                  L"The timer is successfully reset." );
#endif // !_NOT_DEBUGGING

      }

    } else
    {

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"One timer functionality failed! Requested event: "
                                                + Converter::strConverter ( type ) );
#endif // !_NOT_DEBUGGING

    }
  }
  catch (const std::exception& ex)
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};


void Timer::tick ()
{
  // tick and calculate the time between two frames
  try
  {
    if (paused)
      timeDelta = 0; // the elapsed time in a stopped state (for calculations in an idle time)
    else
      if (QueryPerformanceCounter ( ( LARGE_INTEGER*) & timeCurrentFrame ))
      {
        timeDelta = (timeCurrentFrame - timePreviousFrame) * secondsPerCount; // the elapsed time of one frame
        timePreviousFrame = timeCurrentFrame; // preparation for the next tick

        // in case, a negative delta means that the processor goes idle. the cause can be an overflow,
        // a power save mode or the movement of the process to another processor.
        if (timeDelta < 0)
          timeDelta = 0;
      } else
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Timer failed to tick!" );
#endif // !_NOT_DEBUGGING

      }
  }
  catch (const std::exception& ex)
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};
