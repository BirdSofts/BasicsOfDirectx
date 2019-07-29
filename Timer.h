// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,28.07.2019</created>
/// <changed>ʆϒʅ,29.07.2019</changed>
// ********************************************************************************

#ifndef TIMER_H
#define TIMER_H


// timer container (time is measured in counts)
class Timer
{
private:
  long long timeStart; // application start time
  long long timeCurrent; // current frame
  long long timePrevious; // previous frame
  long long timeStopped; // the time of the last stop
  long long timeTotalIdle; // a total of all pauses
  double secondsPerCount; // frequency reciprocal
  double deltaTime; // frequently updated time between two frames
  bool stopped;
public:
  Timer ();
  ~Timer ();
  const double getTotal (); // total minus paused time
  const double& getDelta (); // get delta
  void event ( const char* type ); // start, reset, stop
  void tick (); // the time never stops! (every frame i.e. a cycle of game loop)
};


#endif // !TIMER_H
