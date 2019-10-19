#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

using unsigned_t = unsigned int;
using Unsigned_t = uint64_t;
using float_t    = float;
using double_t   = double;
using Byte_t     = unsigned char;
using PtrByte_t  = Byte_t*;
using PtrFloat_t = float_t*; 

using path_t     = fs::path;
using string_t   = std::string;
