#include "trishul.hpp"
#include <cmath>
#include <cstring>
#include "trishul/CandidateProfilePlot.hpp"

using ccp = CandidateProfilePlot;

ccp::CandidateProfilePlot (float_t _charh) : charh(_charh) {
	std::fill (tr, tr + 6, 0.0f);
	//
	xxfac = 0.1f;	
	// pointer initializing
	fb_fshape  = nullptr;
	fb_tshape  = nullptr;
	fb         = nullptr;
	axtime_ptr = nullptr;
	axfreq_ptr = nullptr;
	// chanout
	chanout = 512;
}

ccp::~CandidateProfilePlot () {
	__safe_free (fb);
	__safe_free (fb_fshape);
	__safe_free (fb_tshape);
	__safe_free (axtime_ptr);
	__safe_free (axfreq_ptr);

}

template<typename It>
void ccp::__ranger (It i, unsigned_t off) {
	auto min = *std::min_element (i, i + off);
	auto max = *std::max_element (i, i + off);

	dd_range = max - min;

	xxmin = min  - (xxfac * dd_range);
	xxmax = max  + (xxfac * dd_range);
}

template<typename T>
void ccp::__safe_malloc (T*& ptr, Unsigned_t s) {
	if (ptr) delete[] ptr;
	ptr = new T[s]();
}

template<typename T>
void ccp::__safe_free (T*& ptr) {
	if (ptr) delete[] ptr;
}

template<typename T>
void ccp::__arange (T* ptr, T start, T step, Unsigned_t size) {
	ptr[0] = start;
	for (Unsigned_t i = 1; i < size; i++) {
		ptr[i] = ptr[i-1] + step;
	}
}

void ccp::Plot (const string_t& filename) {
	cpgbeg (0,"/xwin",1,1); 
	cpgsch (charh); 
	cpgask (1);
	cpgpap (0.0, 0.618);
	////////////////////////////////////////////////////////////////
	// DE-DEDISPERSED WATERFALL
	cpgsvp (0.1, 0.65, 0.1, 0.65);
	cpgswin (
			axtime_ptr[0], axtime_ptr[nsamps-1],
			axfreq_ptr[0], axfreq_ptr[chanout-1]
	);
	cpgbox ("BCN",0.0,0,"BCNV",0.0,0);
	cpgsfs (1);
	cpgctab (
			heat_l.data(), heat_r.data(), 
			heat_g.data(), heat_b.data(), 
			heat_l.size(), constrast, brightness);
	cpgimag (fb, chanout, nsamps,
			1, chanout, 1, nsamps,
			cmin, cmax, tr
	);
	cpgmtxt("B",2.5,.5,0.5,"Time (s)");
	cpgmtxt("L",4,0.5,0.5,"Freq (MHz)");
	// BANDSHAPE
	cpgsfs(1);
	cpgsci(1); // color index
	cpgsvp(0.65, 0.90, 0.1, 0.65); // bandshape
	__ranger (fb_fshape, chanout);
	cpgswin (xxmin, xxmax, axfreq_ptr[0], axfreq_ptr[chanout-1]);
	cpgbox("BCN",0.0,0,"BCV",0.0,0);
	cpgline(chanout, fb_fshape, axfreq_ptr);
	cpgmtxt("B",2.5,.5,0.5,"Intensity (a.u.)"); // group at middle
	cpgmtxt("T",-1*charh,.5,0.5, "Bandshape");
	// TIMESHAPE
	cpgsci (1); // color index
	cpgsvp (0.1, 0.65, 0.65, 0.9); // fscrunched profile 
	__ranger (fb_tshape, nsamps);
	cpgswin (axtime_ptr[0], axtime_ptr[nsamps-1], xxmin, xxmax);
	cpgbox ("BC",0.0,0,"BCNV",0.0,0);
	cpgline (nsamps, axtime_ptr, fb_tshape); 
	cpgmtxt("R",1.2,0.5,0.5,"Intensity (a.u.)");
	cpgmtxt("T",.3,.5,0.5, "De-Dispersed Integrated Profile and Waterfall");
	cpgmtxt("T",2.0,0.0,0.5,group);
	// TEXT
	cpgsci(1); // color index
	cpgsvp(0.65, 0.90, 0.65, 0.9); // Meta data
	txtrow = 0;
	float_t  txtheight = -1.5 * charh;
	char txt[256];
	snprintf(txt, 256, "S/N: %3.2f", sn);
	cpgmtxt("T",txtheight * txtrow++, 0.12, 0.0, txt);
	snprintf(txt, 256, "DM: %3.2f pc/cc", dm);
	cpgmtxt("T",txtheight * txtrow++, 0.12, 0.0, txt);
	snprintf(txt, 256, "Width: %3.2f ms", width*1e3f);
	cpgmtxt("T",txtheight * txtrow++, 0.12, 0.0, txt);
	snprintf(txt, 256, "Peak Time: %4.3f s", peak_time);
	cpgmtxt("T",txtheight * txtrow++, 0.12, 0.0, txt);
	snprintf(txt, 256, "Antenna: ea%02d", stationid);
	cpgmtxt("T",txtheight * txtrow++, 0.12, 0.0, txt);
	snprintf(txt, 256, "Source: %s", name);
	cpgmtxt("T",txtheight * txtrow++, 0.12, 0.0, txt);
	snprintf(txt, 256, "Total time: %3.2f s", duration);
	cpgmtxt("T",txtheight * txtrow++, 0.12, 0.0, txt);
	snprintf(txt, 256, "Tstart(MJD): %3.2f", tstart);
	cpgmtxt("T",txtheight * txtrow++, 0.12, 0.0, txt);
	snprintf(txt, 256, "NBits: %d", nbits);
	cpgmtxt("T",txtheight * txtrow++, 0.12, 0.0, txt);
	snprintf(txt, 256, "NChans: %d", chanin);
	cpgmtxt("T",txtheight * txtrow++, 0.12, 0.0, txt);
	////////////////////////////////////////////////////////////////
	cpgend ();
}

void ccp::Read (const Header_t& h, const Trigger_t& t) {
	// frequency axis
	__safe_malloc (axfreq_ptr, chanout);
	__arange (axfreq_ptr, (float_t)h.fch1, (float_t)h.foff*h.nchans / chanout, chanout);
	chanin = h.nchans;
	// tr
	tr[0] = 0.0f;
	tr[3] = axfreq_ptr[0];
	tr[2] = h.tsamp / 1E6;
	tr[4] = h.foff * h.nchans / chanout;
	cmin  = 0;
	cmax  = pow (2, h.nbits) - 1;
	// shapes
	__safe_malloc (fb_fshape, chanout);
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

void ccp::Read (PtrFloat_t f, const Unsigned_t& _nsamps) {
	nsamps = _nsamps;
	// nsamps
	__safe_malloc (axtime_ptr, nsamps);
	fb_size   = nsamps * chanout;
	__safe_malloc (fb, fb_size);
	__safe_malloc (fb_tshape, nsamps);
	__arange (axtime_ptr, 0.0f, (float_t)( tsamp/1E6 ), nsamps);
	// fscrunch
	Fscrunch (f, nsamps, fb);
	// ABShape
	ABShape (fb, nsamps, chanout, fb_tshape, fb_fshape);
}

