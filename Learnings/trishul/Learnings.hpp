#pragma once
#include "trishul.hpp"

using vs_t = std::vector<string_t>;

class Learnings {
  private:
    // meta
    unsigned_t     nfeatures;
    unsigned_t     nrows;
    // writer
    unsigned_t     totalrows;
    std::ofstream  os;
    void           writer (std::ostream&);
    // feature strings
    vs_t           feature_names;
    vs_t           description;
    vs_t           formats;
    // file legend
    static constexpr char     delimiter = ',';
    static constexpr char     comments  = '#';
    static constexpr char     newline   = '\n';
  protected:
    // features
    std::vector<FloatVector_t> features;
  public:
  // ctor, dtor
  Learnings ( const string_t& filename, const string_t& header )
  : 
  nfeatures (0), nrows(0), totalrows(0) ,
  os (filename) {
    SetupWrite (header);
  }
  ~Learnings () = default;

  // Initialize
  void Initialize (
    const vs_t& , 
    const vs_t& , 
    const vs_t& ); 
    
  // Feature extraction
  virtual void Extract1D (const FloatVector_t&, const Unsigned_t& ) = 0;
  virtual void Extract2D (const FloatVector_t&, const Unsigned_t&, const Unsigned_t&) = 0;

  // Write 
  void SetupWrite (const string_t&);
  void Write () { writer(os); }
};
