#include "trishul/Globals.hpp"

#include <sstream>

string_t change_extension (
  const string_t& in,
  const char * ext) {
  std::stringstream ss;

  // find last dot
  ss << in.substr (0, in.find_last_of(".")) << ext;
  return ss.str();
}

string_t escape_string (const char * in, unsigned_t len) {
  std::stringstream ss;

  for (unsigned_t i = 0; i < len; i++,in++) {
    if (*in == '_') ss << "\\_";
    else ss << *in;
  }
  
  return ss.str();
}
string_t escape_string (const string_t& in) {
  return escape_string (in.c_str(), in.length());
}

string_t change_dir (
  const string_t& in, const string_t& odir) {
  std::stringstream ss; ss << odir << "/";
  ss << in.substr(in.find_last_of ("/")+1);
  return ss.str();
}
