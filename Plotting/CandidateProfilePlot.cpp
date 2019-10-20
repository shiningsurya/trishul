#include "trishul.hpp"
#include <cmath>
#include <cstring>
#include "trishul/CandidateProfilePlot.hpp"

using ccp = CandidateProfilePlot;

constexpr std::array<float,5> ccp::heat_l;
constexpr std::array<float,5> ccp::heat_r;
constexpr std::array<float,5> ccp::heat_g;
constexpr std::array<float,5> ccp::heat_b;

ccp::CandidateProfilePlot (float_t _charh) : charh(_charh) {
	std::fill (tr, tr + 6, 0.0f);
	//
	xxfac = 0.1f;	
	// chanout
	chanout = 512;
}

template<typename It>
void ccp::__ranger (It i, It j) {
	auto min = *std::min_element (i, j);
	auto max = *std::max_element (i, j);

	dd_range = max - min;

	xxmin = min  - (xxfac * dd_range);
	xxmax = max  + (xxfac * dd_range);
}

template<typename T>
void ccp::__arange (std::vector<T>& ptr, T start, T step, Unsigned_t size) {
	ptr.push_back (start);
	for (Unsigned_t i = 1; i < size; i++) {
		ptr.push_back (ptr.back() + step);
	}
}

template<typename T>
void ccp::__zfill (std::vector<T>& ptr, Unsigned_t size) {
	for (Unsigned_t i = 0; i < size; i++) {
		ptr.push_back (0.0f);
	}
}

void ccp::Plot (const string_t& filename) {
	cpgbeg (0,filename.c_str(),1,1); 
	cpgsch (charh); 
	cpgask (1);
	cpgpap (0.0, 0.618);
	////////////////////////////////////////////////////////////////
	// DE-DEDISPERSED WATERFALL
	cpgsvp (0.1, 0.65, 0.1, 0.65);
	cpgswin (
			axtime.front(), axtime.back(),
			axfreq.front(), axfreq.back()
	);
	cpgbox ("BCN",0.0,0,"BCNV",0.0,0);
	cpgsfs (1);
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
	// BANDSHAPE
	cpgsfs(1);
	cpgsci(1); // color index
	cpgsvp(0.65, 0.90, 0.1, 0.65); // bandshape
	__ranger (fb_fshape.cbegin(), fb_fshape.cend());
	cpgswin (xxmin, xxmax, axfreq.front(), axfreq.back());
	cpgbox("BCN",0.0,0,"BCV",0.0,0);
	cpgline(chanout, fb_fshape.data(), axfreq.data());
	cpgmtxt("B",2.5,.5,0.5,"Intensity (a.u.)"); // group at middle
	cpgmtxt("T",-1*charh,.5,0.5, "Bandshape");
	// TIMESHAPE
	cpgsci (1); // color index
	cpgsvp (0.1, 0.65, 0.65, 0.9); // fscrunched profile 
	__ranger (fb_tshape.cbegin(), fb_tshape.cend());
	cpgswin (axtime.front(), axtime.back(), xxmin, xxmax);
	cpgbox ("BC",0.0,0,"BCNV",0.0,0);
	cpgline (nsamps, axtime.data(), fb_tshape.data()); 
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
	__arange (axfreq, (float_t)h.fch1, (float_t)h.foff*h.nchans / chanout, chanout);
	chanin = h.nchans;
	// tr
	tr[0] = 0.0f;
	tr[3] = axfreq.front();
	tr[2] = h.tsamp / 1E6;
	tr[4] = h.foff * h.nchans / chanout;
	cmin  = 0;
	cmax  = pow (2, h.nbits) - 1;
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

void ccp::Read (const FloatVector_t& f, const Unsigned_t& _nsamps) {
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

