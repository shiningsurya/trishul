#include "trishul.hpp"
#include "trishul/Moments.hpp"


int main () {
  Moments tl("lyontest.tfeat", "Lyon features");
  Learnings& ll = tl;
  vs_t vnames{"mean", "variance", "skewness", "kurtosis"};
  vs_t vdesc {"mean of tSN", "variance of tSN", "skewness of tSN", "kurtosis of tSN"};
  vs_t vform {"%5.3f", "%5.3f", "%5.3f", "%5.3f"};
  ll.Initialize (vnames, vdesc, vform);
  FloatVector_t x;
  for (unsigned i = 0; i < 128; i++) x.push_back (i*i);
  ll.Extract1D (x, 0);
  ll.Write();
}
