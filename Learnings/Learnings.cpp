#include "trishul/Learnings.hpp"

using l = Learnings;

void l::Initialize (const vs_t& names, const vs_t& desc, const vs_t& form) {
  feature_names = names;
  description   = desc;
  formats       = form;
  nfeatures     = feature_names.size();
  // prepare features
  // write feature desc
  for (unsigned_t ix = 0; ix < nfeatures; ix++) {
    features.emplace_back ();
    os << comments << feature_names[ix] << delimiter << description[ix] << newline;
  }
}

void l::writer (std::ostream& os) {
  constexpr unsigned_t ffsize = 32;
  char ff[ffsize];
  nrows = features[0].size();
  for (Unsigned_t i = 0; i < nrows; i++) {
    for (unsigned_t ix = 0; ix < nfeatures; ix++) {
      snprintf (ff, ffsize, formats[ix].c_str(), features[ix][i]);
      os << ff << delimiter;
    }
    os << newline;
  }

  totalrows += nrows;
  nrows     =  0;
  for (auto& vv : features) vv.clear ();
}

void l::SetupWrite (const string_t& header) {
  // write header
  os << comments << header << newline;
}
