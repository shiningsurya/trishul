#include "trishul.hpp"
#include "trishul/BSON.hpp"

int main() {
	string_t file("/home/shining/work/vlite/activity400dm/20191013_122557_muos_ea99_dm416.29_sn265.45.fbson");
	// bson
	BSON f;
	const TrishulFormats& tf_f = f;
	if ( !f.ReadFromFile (file) ) {
		std::cerr << "File read failed\n";
		return 1;
	}
	// headers
	Header_t hh; Trigger_t tt;
	f.ReadHeader (hh, tt);
	std::cout << "Header: " << std::endl;
	std::cout << hh;
	std::cout << "Trigger: " << std::endl;
	std::cout << tt;
	std::cout << "NSAMPS: " << std::endl;
	std::cout << f.nsamps << std::endl;

	return 0;
}
