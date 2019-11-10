#include "trishul.hpp"
#include "trishul/Timer.hpp"

void Timer::Start () {
  start = clk::now ();
}

void Timer::Stop () {
  stop = clk::now ();
  // duration
  auto clk_dur = ch::duration_cast<ch::seconds>(stop - start);
  dur = clk_dur.count ();
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
