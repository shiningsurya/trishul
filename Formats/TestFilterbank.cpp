#include "trishul.hpp"
#include "trishul/Filterbank.hpp"

int main () {
  string_t file(
"/home/vlite-master/surya/dankspace/20190711/20190711_221950_muos_ea99.fil"
  );
  //
  Filterbank fb (file);
	const TrishulFormats& tf_f = fb;
	Header_t hh; Trigger_t tt;
	fb.ReadHeader (hh, tt);
	std::cout << "Header: " << std::endl;
	std::cout << hh;
	std::cout << "NSAMPS: " << std::endl;
	std::cout << fb.nsamps / hh.nchans / hh.nbits * 8 << std::endl;
  fb.DiagPrint ();
  return 0;
}
