#pragma once
#include "trishul.hpp"
#include "Learnings.hpp"

class Moments : public Learnings {
  public:
    // ctor
    Moments (const string_t& a, const string_t& b) : 
      Learnings(a,b) {}

    // overrides
    void Extract2D (const FloatVector_t&, const Unsigned_t&, const Unsigned_t& )  override {
      throw TrishulError("Learnings::Moments works with bowtie plane.");
    }
    void Extract1D (const FloatVector_t& x, const Unsigned_t& h) override ;
};

