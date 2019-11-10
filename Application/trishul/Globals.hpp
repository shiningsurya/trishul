#pragma once
#include "trishul.hpp"

// list of functions, constants
// to be available at global scope

// change extension
string_t change_extension (
  const string_t& in,
  const char * ext);

// escape strings
string_t escape_string (const char * in, unsigned_t len);
string_t escape_string (const string_t& in);
