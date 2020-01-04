#include "trishul.hpp"
#include "trishul/Header.hpp"
#include "trishul/BowtiePlot.hpp"
#include "trishul/BSON.hpp"
#include "trishul/PackUnpack.hpp"
#include "trishul/BTIncoherent.hpp"

#include "trishul/Globals.hpp"

#define TIMING
#ifdef TIMING
#include "trishul/Timer.hpp"
#endif 

#define P1 "/users/sbethapu/trishul_stuff/btplots/"
#define P2 "/tmp/"
#define BT

int main(int ac, char* av[]) {
  if (ac != 2) {
    std::cout << "Usage: " << av[0] << " <path-to-fbson>" << std::endl;
    return 0;
  }
  string_t file(av[1]);
  string_t ofile = change_dir (file, P1);
  string_t filename = change_extension (ofile, "");
#ifdef TIMING
  Timer tfdmt ("BTIncoherent");
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
  else if (hh.nbits == 8)
    Unpack8Bit (bdata, data);
	// dedisp
	float_t dwidth = 50;
	unsigned_t dm_count= 256;
	BTIncoherent dd;
	dd.CreatePlan (hh.tsamp, hh.nchans, hh.fch1, hh.foff);
	float_t dmlow = tt.dm - (0.5 * dwidth); 
	float_t dmhigh = tt.dm + (0.5 * dwidth);
	dd.SetDM (dmlow, dmhigh, dm_count);
	Unsigned_t maxdelay = dd.MaxSampDelay ();
	if ( maxdelay >= nsamps) 
	  throw TrishulError ("Nsamps too small for the dm range given!");
	Unsigned_t ddnsamps = nsamps - maxdelay;
	FloatVector_t tdata;
#ifdef TIMING
  tfdmt.Start ();
#endif 
	dd.Execute (data, nsamps, tdata);
#ifdef TIMING
  tfdmt.StopPrint (std::cout);
#endif 
	////////////////////// 
	#ifdef BT
	std::ofstream ofs ("bt.dat");
	std::cout << "dm_low  =" << dmlow << std::endl;
	std::cout << "dm_high =" << dmhigh << std::endl;
	std::cout << "dm_count=" << dm_count << std::endl;
	std::cout << "ddnsamps=" << ddnsamps<< std::endl;
	std::ostream_iterator<float_t> osi(ofs, "\n");
  std::copy (tdata.begin(), tdata.end(), osi);
  ofs.close();
	#else
	// plotting
	BowtiePlot cp;
	cp.SetDM (dd.dm_list);
	cp.Read (hh, tt);
	Unsigned_t plotnsamps = ddnsamps;
	cp.ReadBT (tdata, plotnsamps);
#ifdef TIMING
  tPlot.Start ();
#endif 
	cp.Plot (filename);
#ifdef TIMING
  tPlot.StopPrint (std::cout);
#endif 
  #endif // ndef BT
	//
	return 0;
}
