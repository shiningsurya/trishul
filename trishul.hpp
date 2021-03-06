#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <fstream>
#include <array>
#if __cplusplus >= 201703L
  #define CXX17_FS
  #include <filesystem>
  namespace fs = std::filesystem;
  using path_t = fs::path;
#elif __cplusplus >= 199711L
  #define CXX11_FS
  using path_t = std::string;
  inline std::string operator/(std::string& a, const std::string& b) {
    return  a + "/" + b;
  }
#elif 0
  #define BOOST_FS
  #include <boost/filesystem.hpp>
  namespace fs  = boost::filesystem;
  using path_t  = fs::path;
#endif

using unsigned_t = unsigned int;
using Unsigned_t = uint64_t;
using float_t    = float;
using double_t   = double;
using Byte_t     = unsigned char;
//using PtrByte_t  = Byte_t*;
//using PtrFloat_t = float_t*; 

using string_t   = std::string;


// vector
using FloatVector_t = std::vector<float_t>;
using ByteVector_t = std::vector<Byte_t>;

class TrishulError : public std::exception {
	private:
		string_t msg;
	public:
		TrishulError (const char * _msg) : msg(_msg) {}
		~TrishulError() = default;
		const char * what() const noexcept { return msg.c_str(); }
};
