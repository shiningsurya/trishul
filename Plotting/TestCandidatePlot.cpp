#include "trishul.hpp"
#include "trishul/Header.hpp"
#include "trishul/CandidateProfilePlot.hpp"
#include "trishul/BSON.hpp"
#include "trishul/PackUnpack.hpp"
#include "trishul/Incoherent.hpp"

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
	std::cout << "Header: " << std::endl;
	std::cout << hh;
	Unsigned_t nsamps = f.nsamps / hh.nchans * 8 / hh.nbits;
	std::cout << "\tnsamps=" << nsamps << std::endl;
	// read data
	ByteVector_t bdata;
	Unsigned_t size = f.ReadData (bdata, 0);
	std::cout << "\tsize=" << bdata.size() << std::endl;
	// Unpack
	Unsigned_t fsize = nsamps * hh.nchans;
	FloatVector_t data;
	Unpack2Bit (bdata, data);
	std::cout << std::endl;
	// dedisp
	Incoherent incoh;
	incoh.CreatePlan (hh.tsamp/1E6, hh.nchans, hh.fch1, hh.foff);
	incoh.Delays (tt.dm);
	Unsigned_t ddnsamps = nsamps - incoh.MaxSampDelay ();
	FloatVector_t ddata;
	incoh.Execute (data, nsamps, ddata);
	std::cout << "\tddnsamps=" << ddnsamps << std::endl;
	// plotting
	CandidateProfilePlot cp;
	cp.Read (hh, tt);
	cp.Read (ddata, ddnsamps);
	cp.Plot ("/tmp/haha.png/png");
	//
	return 0;
}
