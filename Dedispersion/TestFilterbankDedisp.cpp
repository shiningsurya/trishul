#include "trishul.hpp"
#include "trishul/Filterbank.hpp"
#include "trishul/Header.hpp"
#include "trishul/BSON.hpp"
#include "trishul/PackUnpack.hpp"
#include "trishul/Dedisp.hpp"
#include "trishul/FilterbankBowtiePlot.hpp"

int main () {
  string_t file(
"/home/vlite-master/surya/dankspace/20190711/20190711_221950_muos_ea99.fil"
  );
  //
  Filterbank fb (file);
	TrishulFormats& tf_f = fb;
	Header_t hh; Trigger_t tt;
	fb.ReadHeader (hh, tt);
	std::cout << "Header: " << std::endl;
	std::cout << hh;
	Unsigned_t total_nsamps = fb.nsamps / hh.nchans / hh.nbits * 8;
	Unsigned_t nsamps_gulp = 1280 * 8;
	Unsigned_t gulp_size  = nsamps_gulp * hh.nchans * hh.nbits / 8;
	std::cout << "NSAMPS:" << total_nsamps << std::endl;
	std::cout << "GULP  :" << nsamps_gulp << std::endl;
	////////////////////// 
	// dedisp
	Dedisp dd;
	dd.CreatePlan (hh.tsamp, hh.nchans, hh.fch1, hh.foff);
	float_t dmlow = 0.0f; float_t dmhigh = 100.0f;
	unsigned_t dm_count= 256;
	dd.SetDM (dmlow, dmhigh, dm_count);
	std::cout << "dm_list initialized" << std::endl;
	Unsigned_t maxdelay = dd.MaxSampDelay ();
	Unsigned_t ddnsamps = nsamps_gulp - maxdelay;
	std::cout << "Maxd  :" << maxdelay << std::endl;
	std::cout << "MaxdT :" << maxdelay*hh.tsamp << std::endl;
	////////////////////// 
	if ( maxdelay >= gulp_size ) 
	  throw TrishulError ("Gulp size too small for the dm range given!");
	////////////////////// 
	// plotting
	FilterbankBowtiePlot fbp(0.65);
	fbp.Read (hh, tt);
	fbp.SetDM (dd.dm_list);
	////////////////////// 
	// containers
	FloatVector_t fbdata;   // Unpacked
	ByteVector_t  ofbdata;  // Overlapped fb data
	FloatVector_t btdata;   // Bowtie 
	////////////////////// 
	// pipeline
	Unsigned_t readbytes;
	string_t tplotname ("/users/sbethapu/trishul_stuff/haha.png/png");
	//string_t tplotname ("?");

	Unsigned_t overlap = 0.2 * nsamps_gulp + maxdelay;
	// overlap of > 1 doesn't make sense
  Unsigned_t overlap_size = overlap * hh.nchans * hh.nbits / 8;
  //////////////////////
  // resizes
	ofbdata.resize (gulp_size + overlap_size, 0);
	fbdata.resize (hh.nchans * (nsamps_gulp + overlap), 3.0f);
	btdata.resize (dm_count * (ddnsamps + overlap),0.0f);
	//////////////////////////////////////////////////////
	// First read
  readbytes = tf_f.ReadData (ofbdata, gulp_size);
  // unpack
  Unpack2Bit (ofbdata, fbdata);

  std::cout << "[!!] Executing with=" << nsamps_gulp ;
  std::cout << " overlap=" << 0 << std::endl;
  // dedisp
  dd.Execute (ofbdata, hh.nbits, nsamps_gulp, btdata);

  // this is where I'd execute my pipeline 
  // if I had one!!!
  // for now plot
  fbp.ReadFB (fbdata, ddnsamps, 0L);
  fbp.ReadBT (btdata, ddnsamps, 0L);
  fbp.Plot (tplotname);
  return 0;

  // overlap
  auto _i = std::copy (ofbdata.begin() + gulp_size - overlap_size, ofbdata.begin() + gulp_size, ofbdata.begin());
  if (_i != ofbdata.begin() + overlap_size ) {
    std::cerr << "First overlap not matching!" << std::endl;
  }


  // read for next iteration
  readbytes = tf_f.ReadData (ofbdata, gulp_size, overlap_size);
  while (readbytes == gulp_size) {
    // had a full read
    // unpack
    Unpack2Bit (ofbdata, fbdata);

    std::cout << "[!!] Executing with=" << nsamps_gulp ;
    std::cout << " overlap=" << overlap << std::endl;
    // dedisp
    dd.Execute (ofbdata, hh.nbits, nsamps_gulp+overlap, btdata);

    // this is where I'd execute my pipeline 
    // if I had one!!!
    // for now plot
    fbp.ReadFB (fbdata, ddnsamps + overlap, overlap-maxdelay);
    fbp.ReadBT (btdata, ddnsamps + overlap, overlap-maxdelay);
    fbp.Plot (tplotname);

    // overlap
    auto _i = std::copy (
        ofbdata.begin() + gulp_size, ofbdata.end(), 
        ofbdata.begin()
        );
    if (_i != ofbdata.begin() + overlap_size ) {
      std::cerr << "Running overlap not matching!" << std::endl;
    }
    // read for next iteration
    readbytes = tf_f.ReadData (ofbdata, gulp_size, overlap_size);
  }
  if (readbytes != 0 || readbytes <= gulp_size) {
    // we have some more data less than gulpsize
    Unsigned_t residue = gulp_size - readbytes;
    Unsigned_t rsamps  = residue * 8 / hh.nchans / hh.nbits;

    // pipeline execution again
    std::cout << "[!!] Executing with " << overlap+rsamps << std::endl;
    // dedisp
    dd.Execute (ofbdata, hh.nbits, overlap+rsamps, btdata);

    // plot
    if (rsamps >= maxdelay) {
      fbp.ReadFB (fbdata, rsamps - maxdelay, overlap-maxdelay);
      fbp.ReadBT (btdata, rsamps - maxdelay, overlap-maxdelay);
      fbp.Plot (tplotname);
    }
    else {
      std::cout << "[**] last iteration is left out!" << std::endl;

    }

    // should reach EOD now
    readbytes = tf_f.ReadData (ofbdata, gulp_size, overlap_size);
  }
  std::cout << "last read retcode=" << readbytes << std::endl;
  return 0;
}
