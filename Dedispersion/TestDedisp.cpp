#include "trishul.hpp"
#include "trishul/Header.hpp"
#include "trishul/BSON.hpp"
#include "trishul/PackUnpack.hpp"
#include "trishul/Dedisp.hpp"

int main() {
	string_t file("/home/vlite-master/surya/analysis/ifrb/wtf/20191017_210915_muos_ea11_dm80.13_sn28.75.fbson");
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
	Unsigned_t nsamps = f.nsamps / hh.nchans * 8 / hh.nbits;
	std::cout << "Header: " << std::endl;
	std::cout << hh;
	// read data
	ByteVector_t bdata;
	Unsigned_t size = f.ReadData (bdata, 0);
	// Unpack
	FloatVector_t data;
	Unpack2Bit (bdata, data);
	std::cout << std::endl;
	// dedisp
	Dedisp dd;
	dd.CreatePlan (hh.tsamp/1E6, hh.nchans, hh.fch1, hh.foff);
	float_t dmlow = 0.0f; float_t dmhigh = 100.0f;
	unsigned_t dm_count= 256;
	dd.SetDM (dmlow, dmhigh, dm_count);
	std::cout << "dm_list initialized" << std::endl;
	Unsigned_t ddnsamps = nsamps - dd.MaxSampDelay ();
	std::cout << "\tnsamps="<< nsamps << std::endl;
	std::cout << "\tddnsamps=" << ddnsamps << std::endl;
	FloatVector_t ddata;
	dd.Execute (data, nsamps, ddata);
	//
	return 0;
}
