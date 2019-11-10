#include "trishul.hpp"
#include "trishul/Header.hpp"
#include "trishul/CandidatePlot.hpp"
#include "trishul/BSON.hpp"
#include "trishul/PackUnpack.hpp"
#include "trishul/Incoherent.hpp"
#include "trishul/Dedisp.hpp"

#define TIMING
#ifdef TIMING
#include "trishul/Timer.hpp"
#endif 

int main() {
#ifdef TIMING
  Timer tincoh ("Incoherent");
  Timer tdedisp ("Dedisp");
  Timer tPlot ("mglPlot");
#endif 
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
	std::cout << "tsamp=" << hh.tsamp << std::endl;
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
#ifdef TIMING
  tincoh.Start ();
#endif 
	incoh.Execute (data, nsamps, ddata);
#ifdef TIMING
  tincoh.StopPrint (std::cout);
#endif 
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
	std::cout << "ddnsamps  :" << ddnsamps<< std::endl;
	std::cout << "Maxd  :" << maxdelay << std::endl;
	std::cout << "MaxdT :" << maxdelay*hh.tsamp << std::endl;
	std::cout << "DMsize:" << dd.dm_list.size() << std::endl;
	FloatVector_t tdata (dm_count * ddnsamps);
#ifdef TIMING
  tdedisp.Start ();
#endif 
	dd.Execute (bdata, hh.nbits, nsamps, tdata);
#ifdef TIMING
  tdedisp.StopPrint (std::cout);
#endif 
	////////////////////// 
	if ( maxdelay >= nsamps) 
	  throw TrishulError ("Nsamps too small for the dm range given!");
	// plotting
	CandidatePlot cp;
	cp.SetDM (dd.dm_list);
	std::cout << "DMsize:" << dd.dm_list.size() << std::endl;
	cp.Read (hh, tt);
	Unsigned_t plotnsamps = ddnsamps;
	cp.ReadFB (ddata, plotnsamps);
	cp.ReadBT (tdata, plotnsamps);
#ifdef TIMING
  tPlot.Start ();
#endif 
	cp.Plot ("/users/sbethapu/trishul_stuff/mgl_candplot");
#ifdef TIMING
  tPlot.StopPrint (std::cout);
#endif 
	//
	return 0;
}
