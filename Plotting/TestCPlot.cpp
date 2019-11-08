#include "trishul.hpp"
#include "trishul/Header.hpp"
#include "trishul/CandidatePlot.hpp"
#include "trishul/BSON.hpp"
#include "trishul/PackUnpack.hpp"
#include "trishul/Incoherent.hpp"
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
	// containers
	ByteVector_t  bdata;
	FloatVector_t ddata;
	// headers
	Header_t hh; Trigger_t tt;
	f.ReadHeader (hh, tt);
	std::cout << "Header: " << std::endl;
	std::cout << hh;
	Unsigned_t nsamps = f.nsamps / hh.nchans * 8 / hh.nbits;
	std::cout << "\tnsamps=" << nsamps << std::endl;
	// read data
	Unsigned_t size = f.ReadData (bdata, 0);
	std::cout << "\tsize=" << bdata.size() << std::endl;
	// Unpack
	Unsigned_t fsize = nsamps * hh.nchans;
	FloatVector_t data (fsize);
	Unpack2Bit (bdata, data);
	// incoh
	Incoherent incoh;
	incoh.CreatePlan (hh.tsamp, hh.nchans, hh.fch1, hh.foff);
	incoh.Delays (tt.dm);
	incoh.Execute (data, nsamps, ddata);
	// dedisp
	float_t dwidth = 50;
	Dedisp dd;
	dd.CreatePlan (hh.tsamp, hh.nchans, hh.fch1, hh.foff);
	float_t dmlow = tt.dm - (0.5 * dwidth); 
	float_t dmhigh = tt.dm + (0.5 * dwidth);
	unsigned_t dm_count= 256;
	std::cout << "dmlow=" << dmlow  << std::endl;
	std::cout << "dmhigh" << dmhigh <<std::endl;
	dd.SetDM (dmlow, dmhigh, dm_count);
	Unsigned_t maxdelay = dd.MaxSampDelay ();
	Unsigned_t ddnsamps = nsamps - maxdelay;
	std::cout << "Maxd  :" << maxdelay << std::endl;
	std::cout << "MaxdT :" << maxdelay*hh.tsamp << std::endl;
	std::cout << "DMsize:" << dd.dm_list.size() << std::endl;
	FloatVector_t tdata (dm_count * ddnsamps);
	dd.Execute (bdata, hh.nbits, nsamps, tdata);
	////////////////////// 
	if ( maxdelay >= nsamps) 
	  throw TrishulError ("Nsamps too small for the dm range given!");
	// plotting
	CandidatePlot cp;
	cp.SetDM (dd.dm_list);
	cp.Read (hh, tt);
	Unsigned_t plotnsamps = maxdelay;
	cp.ReadFB (ddata, plotnsamps);
	cp.ReadBT (tdata, plotnsamps);
	cp.Plot ("/users/sbethapu/haha.png/png");
	//
	return 0;
}
