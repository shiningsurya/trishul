#include "trishul.hpp"
// FBSON
#include "trishul/Header.hpp"
#include "trishul/BSON.hpp"
#include "trishul/PackUnpack.hpp"
// BowTie help
#include "trishul/BTIncoherent.hpp"
#define TIMING
#ifdef TIMING
#include "trishul/Timer.hpp"
#endif 
// Feature extraction
#include "trishul/Moments.hpp"

int main(int ac, char* av[]) {
  if (ac != 2) {
    std::cout << "Usage: " << av[0] << " <path-to-fbson-list>" << std::endl;
    return 0;
  }
#ifdef TIMING
  Timer tfdmt ("BTIncoherent");
  Timer tslice ("Slicing");
  Timer tmom ("Moments");
#endif 
	//////////// ML
  Moments tl(string_t(av[1])+string_t(".tfeat"), "Lyon features");
  Learnings& ll = tl;
  vs_t vnames{
    "tmean", "tvariance", "tskewness", "tkurtosis",
    "dmean", "dvariance", "dskewness", "dkurtosis"
  };
  vs_t vdesc {
    "mean of tSN", "variance of tSN", "skewness of tSN", "kurtosis of tSN",
    "mean of dSN", "variance of dSN", "skewness of dSN", "kurtosis of dSN"
  };
  vs_t vform (8,"%5.3f");
  ll.Initialize (vnames, vdesc, vform);
  // dedisp initialize
	float_t dwidth = 50;
	unsigned_t dm_count= 256;
  //////////// Reading
  std::ifstream list(av[1]);
  string_t file;
  unsigned_t counter = 0;
  while (std::getline (list, file)) {
    if (counter++ >= 3) {
      ll.Write();
      counter = 0;
    }
	  BSON f;
    if ( !f.ReadFromFile (file) ) {
      std::cerr << "File read failed f=" << file << std::endl;
      continue;
    }
    ///////////  Bowtie
    // containers
    ByteVector_t  bdata;
    Header_t hh; Trigger_t tt;
    f.ReadHeader (hh, tt);
    Unsigned_t nsamps = f.nsamps / hh.nchans * 8 / hh.nbits;
    // read data
    f.ReadData (bdata, 0);
    // Unpack
    Unsigned_t fsize = nsamps * hh.nchans;
    FloatVector_t data (fsize);
    if (hh.nbits == 2)
      Unpack2Bit (bdata, data);
    else if (hh.nbits == 8)
      Unpack8Bit (bdata, data);
    // dedisp
    BTIncoherent dd;
    dd.CreatePlan (hh.tsamp, hh.nchans, hh.fch1, hh.foff);
    float_t dmlow = tt.dm - (0.5 * dwidth); 
    float_t dmhigh = tt.dm + (0.5 * dwidth);
    dd.SetDM (dmlow, dmhigh, dm_count);
    Unsigned_t maxdelay = dd.MaxSampDelay ();
    if ( maxdelay >= nsamps) {
      std::cerr << "Nsamps not enough f=" << file << std::endl;
      continue;
    }
    Unsigned_t ddnsamps = nsamps - maxdelay;
    FloatVector_t tdata;
#ifdef TIMING
    tfdmt.Start ();
#endif 
    dd.Execute (data, nsamps, tdata);
#ifdef TIMING
    tfdmt.StopPrint (std::cout);
#endif 
    //////////// slice
#ifdef TIMING
    tslice.Start ();
#endif 
    FloatVector_t   tbt(ddnsamps), dbt(dm_count);
    Unsigned_t pt = tt.peak_time / hh.tsamp;
    Unsigned_t dc = 0.5 * dm_count;
    for (Unsigned_t id = 0; id < dm_count; id++)
      dbt[id] = tdata [pt + ddnsamps*id];
    for (Unsigned_t is = 0; is < ddnsamps; is++)
      tbt[is] = tdata [is + ddnsamps*dc];
#ifdef TIMING
    tslice.StopPrint (std::cout);
#endif 
#ifdef TIMING
    tmom.Start ();
#endif 
    ll.Extract1D (tbt, 0);
    ll.Extract1D (dbt, 4);
#ifdef TIMING
    tmom.StopPrint (std::cout);
#endif 
  } // main loop
  ll.Write();
  return 0;
}
