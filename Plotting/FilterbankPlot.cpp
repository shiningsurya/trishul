#include "trishul/FilterbankPlot.hpp"

using fp = FilterbankPlot;

constexpr std::array<float,5> ccp::heat_l;
constexpr std::array<float,5> ccp::heat_r;
constexpr std::array<float,5> ccp::heat_g;
constexpr std::array<float,5> ccp::heat_b;

fp::FilterbankPlot (unsigned_t nrow_, float_t rowdur_,  float_t _charh) : 
	nrows(nrow_), rowdur(rowdur_),
	charh(_charh) {
	std::fill (tr, tr + 6, 0.0f);
	//
	xxfac = 0.1f;	
	// chanout
	chanout = 512;
}

template<typename It>
void fp::__ranger (It i, It j) {
	auto min = *std::min_element (i, j);
	auto max = *std::max_element (i, j);

	dd_range = max - min;

	xxmin = min  - (xxfac * dd_range);
	xxmax = max  + (xxfac * dd_range);
}

template<typename T>
void fp::__arange (std::vector<T>& ptr, T start, T step, Unsigned_t size) {
	ptr.push_back (start);
	for (Unsigned_t i = 1; i < size; i++) {
		ptr.push_back (ptr.back() + step);
	}
}

template<typename T>
void fp::__zfill (std::vector<T>& ptr, Unsigned_t size) {
	for (Unsigned_t i = 0; i < size; i++) {
		ptr.push_back (0.0f);
	}
}

void fp::Plot (const string_t& filename) {
	cpgbeg (0,filename.c_str(),1,1); 
	cpgsch (charh); 
	cpgask (0);
	cpgpap (0.0, 0.618);
	////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////
	cpgend ();
}
