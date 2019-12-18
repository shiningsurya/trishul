#include "trishul.hpp"
#include "trishul/Header.hpp"
#include "trishul/CandidatePlot.hpp"
#include "trishul/BSON.hpp"
#include "trishul/PackUnpack.hpp"
#include "trishul/Incoherent.hpp"
#include "trishul/Dedisp.hpp"

#include "trishul/Globals.hpp"

#define TIMING
#ifdef TIMING
#include "trishul/Timer.hpp"
#endif 

//#define DEBUG

int main(int ac, char* av[]) {
  if (ac != 2) {
    std::cout << "Usage: " << av[0] << " <path-to-fbson>" << std::endl;
    return 0;
  }
  string_t file(av[1]);
  string_t ofile = change_dir (file, "/users/sbethapu/trishul_stuff/cands/");
  string_t filename = change_extension (ofile, "");
#ifdef TIMING
  Timer tincoh ("Incoherent");
  Timer tdedisp ("Dedisp");
  Timer tPlot ("mglPlot");
#endif 
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
	Unsigned_t nsamps = f.nsamps / hh.nchans * 8 / hh.nbits;
	// read data
	Unsigned_t size = f.ReadData (bdata, 0);
	// Unpack
	Unsigned_t fsize = nsamps * hh.nchans;
	FloatVector_t data (fsize);
	if (hh.nbits == 2)
    Unpack2Bit (bdata, data);
  else if (hh.nbits == 4)
    Unpack4Bit (bdata, data);
  else if (hh.nbits == 8)
    Unpack8Bit (bdata, data);
  else {
    throw TrishulError ("Nbits not understood!");
  }
  #ifdef DEBUG
  std::cout << "size=" << size << std::endl;
  std::cout << "bdata.size()=" << bdata.size() << std::endl;
  #endif
	// incoh
	Incoherent incoh;
	incoh.CreatePlan (hh.tsamp, hh.nchans, hh.fch1, hh.foff);
	incoh.Delays (tt.dm);
	// dedisp
	float_t dwidth = 50;
	unsigned_t dm_count= 256;
	Dedisp dd; dd.SetDevice (1);
	dd.CreatePlan (hh.tsamp, hh.nchans, hh.fch1, hh.foff);
	float_t dmlow = tt.dm - (0.5 * dwidth); 
	float_t dmhigh = tt.dm + (0.25 * dwidth);
	#ifdef DEBUG
  std::cout << "dmlow=" << dmlow<< std::endl;
  std::cout << "dmhigh=" << dmhigh<< std::endl;
	#endif
	dd.SetDM (dmlow, dmhigh, dm_count);
	//for (auto& _x : dd.dm_list) std::cout << " x "<< _x << std::endl;
	const Unsigned_t maxdelay = dd.MaxSampDelay ();
	Unsigned_t ddnsamps = nsamps - maxdelay;
  #ifdef DEBUG
  std::cout << "maxdelay=" << maxdelay<< std::endl;
  std::cout << "nsamps=" << nsamps<< std::endl;
  std::cout << "maxdelaytime=" << maxdelay*hh.tsamp<< std::endl;
  std::cout << "nsampstime  =" << nsamps*hh.tsamp<< std::endl;
  #endif
	FloatVector_t tdata (dm_count * ddnsamps);
  #ifdef DEBUG
  std::cout << "tdata.size()=" << tdata.size()<< std::endl;
  #endif
#ifdef TIMING
  tincoh.Start ();
#endif 
	incoh.Execute (data, ddnsamps, ddata);
#ifdef TIMING
  tincoh.StopPrint (std::cout);
#endif 
#ifdef TIMING
  tdedisp.Start ();
#endif 
	if ( maxdelay >= nsamps) 
	  throw TrishulError ("Nsamps too small for the dm range given!");
	dd.Execute (bdata, hh.nbits, nsamps, tdata);
#ifdef TIMING
  tdedisp.StopPrint (std::cout);
#endif 
	////////////////////// 
	// plotting
	CandidatePlot cp;
	cp.SetDM (dd.dm_list);
	cp.Read (hh, tt);
	Unsigned_t plotnsamps = ddnsamps;
	cp.ReadFB (ddata, plotnsamps);
	cp.ReadBT (tdata, plotnsamps);
#ifdef TIMING
  tPlot.Start ();
#endif 
	cp.Plot (filename);
#ifdef TIMING
  tPlot.StopPrint (std::cout);
#endif 
	//
	return 0;
}
