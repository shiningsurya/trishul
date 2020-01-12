#include "trishul.hpp"
#include "trishul/Timer.hpp"

void Timer::Start () {
  start = clk::now ();
}

void Timer::Stop () {
  stop = clk::now ();
  // clock 
  auto sclk_dur = ch::duration_cast<ch::seconds>(stop - start);
  auto mclk_dur = ch::duration_cast<ch::milliseconds>(stop - start);
  //ticks
  dur_sec  = sclk_dur.count ();
  dur_msec = mclk_dur.count ();
  dur = dur_sec + (dur_msec/1000.0);
}

std::ostream& operator<< (std::ostream& os, const Timer& t) {
  os << "Timer::" << t.sid;
  os << " ";
  os << t.dur << std::endl;; 
  return os;
}

void Timer::StopPrint(std::ostream& os) {
  Stop ();
  os << *this;
}
