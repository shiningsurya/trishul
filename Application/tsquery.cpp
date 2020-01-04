#include "trishul.hpp"
#include "trishul/BSON.hpp"

int main (int ac, char * av[]) {
  if (ac != 2) {
    std::cout << "Usage: " << av[0] << " <path-to-fbson>" << std::endl;
    return 0;
  }
  string_t file(av[1]);

	BSON f;
	if ( !f.ReadFromFile (file) ) {
		std::cerr << "File read failed\n";
		return 1;
	}
	const TrishulFormats& tf_f = f;
	// headers
	Header_t hh; Trigger_t tt;
	f.ReadHeader (hh, tt);
	//
	std::cout << file << "\t" << tt.width*1e3f << std::endl;
  return 0;
}
