#include "trishul.hpp"
#include "trishul/Header.hpp"
#include "trishul/CandidatePlot.hpp"
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
	float_t dwidth = 15;
	Dedisp dd;
	dd.CreatePlan (hh.tsamp/1E6, hh.nchans, hh.fch1, hh.foff);
	float_t dmlow = tt.dm - (0.5 * dwidth); 
	float_t dmhigh = tt.dm + (0.5 * dwidth);
	unsigned_t dm_count= 256;
	dd.SetDM (dmlow, dmhigh, dm_count);
	Unsigned_t maxdelay = dd.MaxSampDelay ();
	Unsigned_t ddnsamps = nsamps - maxdelay;
	std::cout << "Maxd  :" << maxdelay << std::endl;
	std::cout << "MaxdT :" << maxdelay*hh.tsamp << std::endl;
	////////////////////// 
	if ( maxdelay >= nsamps) 
	  throw TrishulError ("Nsamps too small for the dm range given!");
	// plotting
	CandidateProfilePlot cp;
	cp.Read (hh, tt);
	cp.ReadFB (ddata, ddnsamps);
	cp.Plot ("/tmp/haha.png/png");
	//
	return 0;
}
