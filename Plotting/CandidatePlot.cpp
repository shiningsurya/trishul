#include "trishul.hpp"
#include "trishul/CandidatePlot.hpp"

using cp = CandidatePlot;

cp::CandidatePlot (float_t charh_) : charh(charh_) {
	std::fill (tr_fb, tr_fb + 6, 0.0f);
	std::fill (tr_bt, tr_bt + 6, 0.0f);
	// __ranger factor
	xxfac = 0.1f;	
	// chanout
	chanout = 512;
	// count
	count = 0;
	// times
	tleft = tright = 0.0f;
}

void cp::Plot (const string_t& filename) {
	cpgbeg (0,filename.c_str(),1,1); 
	cpgsch (charh); 
	cpgask (0);
	cpgpap (0.0, 0.618);
	////////////////////////////////////////////////////////////////
	// De-dispersed waterfall 
	cpgsvp (0.1, 0.5, 0.1, 0.5);
	cpgswin (
			axtime.front(), axtime.back(),
			axfreq.front(), axfreq.back()
	);
	cpgbox ("BCN",0.0,0,"BCNV",0.0,0);
	cpgctab (
			heat_l.data(), heat_r.data(), 
			heat_g.data(), heat_b.data(), 
			heat_l.size(), constrast, brightness);
	cpgimag (fb.data(), chanout, nsamps,
			1, chanout, 1, nsamps,
			cmin, cmax, tr
	);
	cpgmtxt("B",2.5,.5,0.5,"Time (s)");
	cpgmtxt("L",4,0.5,0.5,"Freq (MHz)");
	// Bowtie
	cpgsvp(0.1, 0.5, 0.5, 0.9);
	cpgswin (
			axtime.front(), axtime.back(),
			axdm.front(), axdm.back()
	);
	cpgbox ("BCN",0.0,0,"BCNV",0.0,0);
	cpgctab (
			anrb_l.data(), anrb_r.data(), 
			anrb_g.data(), anrb_b.data(), 
			anrb_l.size(), constrast, brightness);
	__ranger (bt, nsamps*axdm.size(), xxmin, xxmax);
	cpgimag (bt.data(), chanout, nsamps,
			1, chanout, 1, nsamps,
			xxmin, xxmax, tr
	);
	cpgmtxt("B",2.5,.5,0.5,"Time (s)");
	cpgmtxt("L",4,0.5,0.5,"Freq (MHz)");
	// Bandshape
	cpgsvp(0.4, 0.5, 0.5, 0.9);
	__ranger (fb_fshape.cbegin(), fb_fshape.cend(), xxmin, xxmax);
	cpgswin (xxmin, xxmax, axfreq.front(), axfreq.back());
	cpgbox("BCN",0.0,0,"BCV",0.0,0);
	cpgline(chanout, fb_fshape.data(), axfreq.data());
	cpgmtxt("B",2.5,.5,0.5,"Intensity (a.u.)"); // group at middle
	cpgmtxt("T",-1*charh,.5,0.5, "Bandshape");
	// Bowtie-timeshape
	// Profile
	// text
	////////////////////////////////////////////////////////////////
	cpgend ();
}

void cp::Read (const Header_t& h, const Trigger_t& t) {
	// frequency axis
	__arange (axfreq, (float_t)h.fch1, (float_t)h.foff*h.nchans / chanout, chanout);
	chanin = h.nchans;
	// tr - fb
	tr[0] = 0.0f;
	tr[3] = axfreq.front();
	tr[2] = h.tsamp / 1E6;
	tr[4] = h.foff * h.nchans / chanout;
	cmin  = 0;
	cmax  = pow (2, h.nbits) - 1;
	// tr - bt
	tr_bt[0] = 0.0f;
	tr_bt[2] = h.tsamp;
	// parameters
	sn = t.sn;
	dm = t.dm;
	width = t.width;
	tsamp = h.tsamp;
	tstart = h.tstart;
	peak_time = t.peak_time;
	stationid = h.stationid;
	strcpy (name, h.name);
	strcpy (group, h.group);
	duration = std::ceil (t.i1 - t.i0);
	nbits = h.nbits;
}

void cp::ReadFB (const FloatVector_t& f, const Unsigned_t& _nsamps, const Unsigned_t& _offset) {
	// offset should always be zero
	static_assert (_offset == 0);
	// nsamps
	nsamps = _nsamps;
	// reserve
	__zfill (fb_tshape, nsamps);
	__zfill (fb_fshape, chanout);
	// nsamps
	fb_size   = nsamps * chanout;
	__zfill (fb, fb_size);
	// time axis
	__arange (axtime, 0.0f, (float_t)( tsamp/1E6 ), nsamps);
	// fscrunch
	Fscrunch (f, nsamps, fb);
	// ABShape
	ABShape (fb, nsamps, chanout, fb_tshape, fb_fshape);
}

void cp::ReadBT (const FloatVector_t& f, 
  const Unsigned_t& _nsamps,
  const Unsigned_t& _offset) {
	// offset should always be zero
	static_assert (_offset == 0);
	// XXX It is called after ReadFB has been called
	if (_nsamps != nsamps) {
		std::cerr << "FilterbankBowtiePlot::ReadFB read different." << std::endl;
		nsamps = std::min (nsamps, _nsamps);
	}
	// resize
	__zfill (bt_tshape, nsamps);
	__zfill (bt_fshape, axdm.size());
	// copy 
	bt = f;
	// ABShape
	ABMaxShape (bt, nsamps, axdm.size(), bt_tshape, bt_fshape);
}
