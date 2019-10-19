#include "trishul.hpp"
#include "trishul/Header.hpp"

std::ostream& operator<<(std::ostream& os, const Header_t& f) {
	os << std::fixed;
	os << "Antenna      ea" << f.stationid<< std::endl;
	os << "Nchans=" << f.nchans << "  Nbits=" << f.nbits << std::endl;
	os << "Source       " << f.name << std::endl;
	return os;
};

std::ostream& operator<<(std::ostream& os, const Trigger_t& f) {
	os << std::fixed;
	os << "S/N          " << f.sn << std::endl;
	os << "DM (pc/cc)   " << f.dm << std::endl;
	os << "Width (ms)   " << f.width*1e3 << std::endl;
	os << "Peak_time    " << f.peak_time << std::endl;
	return os;
};

