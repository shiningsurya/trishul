#include "trishul/Moments.hpp"

#include <cmath>

using m = Moments;

// Following Lyon et.al. 
// Computing first four central moments
// mean, variance, skewness, and kurtosis
// of S/N as a function of time 
// of S/N as a function of DM

// The algorithm first computes non central moments 
// and then uses algebra to get the central moments

void m::Extract1D (const FloatVector_t& s, const Unsigned_t& id) {
  // non central moments
  // central moments
  float_t    m1=0.0f, cm1=0.0f;
  float_t    m2=0.0f, cm2=0.0f;
  float_t    m3=0.0f, cm3=0.0f;
  float_t    m4=0.0f, cm4=0.0f;

  // loop
  auto sl = s.size();
  for (unsigned_t i = 0; i < sl; i++) {
    m1 += s[i];
    m2 += std::pow (s[i], 2);
    m3 += std::pow (s[i], 3);
    m4 += std::pow (s[i], 4);
  }
  m1 /= sl;
  m2 /= sl;
  m3 /= sl;
  m4 /= sl;

  // algebra
  cm1 = m1;
  cm2 = m2 - std::pow (cm1,2);
  cm3 = ( m3 - (3*cm1*cm2) - std::pow (cm1,3) )/std::pow (cm2, 1.5);
  cm4 = ( m4 -(3*std::pow(cm1,4)) +(6*m2*std::pow(cm1,2)) -(4*cm1*m3))/std::pow (cm2, 2);
  cm4 -= 3; // excess kurtosis

  // add
  features[id].push_back   ( cm1 );
  features[id+1].push_back ( cm2 );
  features[id+2].push_back ( cm3 );
  features[id+3].push_back ( cm4 );
}
