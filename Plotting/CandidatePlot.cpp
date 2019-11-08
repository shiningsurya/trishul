#include "trishul.hpp"
#include <cmath>
#include "trishul/CandidatePlot.hpp"

using cp = CandidatePlot;

constexpr std::array<float,5> cp::heat_l;
constexpr std::array<float,5> cp::heat_r;
constexpr std::array<float,5> cp::heat_g;
constexpr std::array<float,5> cp::heat_b;
constexpr std::array<float,10> cp::anrb_l;
constexpr std::array<float,10> cp::anrb_r;
constexpr std::array<float,10> cp::anrb_g;
constexpr std::array<float,10> cp::anrb_b;



cp::CandidatePlot (float_t charh_) : charh(charh_) {
	std::fill (tr_fb, tr_fb + 6, 0.0f);
	std::fill (tr_bt, tr_bt + 6, 0.0f);
	// __ranger factor
	xxfac = 0.1f;	
	// chanout
	chanout = 512;
	// times
	tleft = tright = 0.0f;
}

void cp::Plot (const string_t& filename) {
	cpgbeg (0,filename.c_str(),1,1); 
	cpgsch (charh); 
	cpgask (0);
	cpgpap (10.25, 8.5/11.0);
	cpgstbg (2);
	////////////////////////////////////////////////////////////////
	// De-dispersed waterfall 
	cpgsvp (0.05, 0.35, 0.1, 0.6);
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
			cmin, cmax, tr_fb
	);
	cpgmtxt("B",2.5,.5,0.5,"Time (s)");
	cpgmtxt("L",4,0.5,0.5,"Freq (MHz)");
	// Bowtie
	cpgsvp(0.55, 0.9, 0.1, 0.4);
	cpgswin (
			axtime.front(), axtime.back(),
			axdm.front(), axdm.back()
	);
	cpgbox ("BCN",0.0,0,"BCNV",0.0,0);
	cpgctab (
			anrb_l.data(), anrb_r.data(), 
			anrb_g.data(), anrb_b.data(), 
			anrb_l.size(), constrast, brightness);
	__ranger (bt.cbegin(), nsamps*axdm.size(), xxmin, xxmax);
	cpgimag (bt.data(), axdm.size(), nsamps,
			1, axdm.size(), 1, nsamps,
			xxmin, xxmax, tr_bt
	);
	cpgmtxt("B",2.5,.5,0.5,"Time (s)");
	cpgmtxt("L",4,0.5,0.5,"Freq (MHz)");
	// Bandshape
	cpgsvp(0.35, 0.4, 0.1, 0.6);
	__ranger (fb_fshape.cbegin(), fb_fshape.cend(), xxmin, xxmax);
	cpgswin (xxmin, xxmax, axfreq.front(), axfreq.back());
	cpgbox("BCN",0.0,0,"BCV",0.0,0);
	cpgline(chanout, fb_fshape.data(), axfreq.data());
	cpgmtxt("B",2.5,.5,0.5,"Intensity (a.u.)"); // group at middle
	cpgmtxt("T",1.0,.5,0.5, "Bandshape");
	// Bowtie-bshape
	cpgsvp(0.45, 0.55, 0.1, 0.4);
	__ranger (bt_fshape.cbegin(), bt_fshape.cend(), xxmin, xxmax);
	cpgswin (xxmin, xxmax, axdm.front(), axdm.back());
	cpgbox("BCM",0.0,0,"BCV",0.0,0);
	cpgline(axdm.size(), bt_fshape.data(), axdm.data());
	cpgmtxt("B",2.5,.5,0.5,"Max per DM"); 
	// Profile
	cpgsvp(0.05, 0.35, 0.6, 1.);
	__ranger (fb_tshape.cbegin(), fb_tshape.cend(), xxmin, xxmax);
	cpgswin (axtime.front(), axtime.back(), xxmin, xxmax);
	cpgbox ("A",0.0,0,"NV",0.0,0);
	cpgline (nsamps, axtime.data(), fb_tshape.data()); 
	cpgmtxt("R",1.2,0.5,0.5,"Intensity (a.u.)");
	cpgmtxt("T",.3,.5,0.5, "De-Dispersed Integrated Profile and Waterfall");
	//cpgmtxt("T",2.0,0.0,0.5,group);
	// text
	cpgsvp(0.45, 0.9, 0.4, 1.0); // Meta data
	txtrow = 0;
	float_t  txtheight = -1.5 * charh;
	char txt[256];
	cpgscf (2);
	cpgsch (3.0);
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

void cp::Read (const Header_t& h, const Trigger_t& t) {
	// frequency axis
	__arange (axfreq, (float_t)h.fch1, (float_t)h.foff*h.nchans / chanout, chanout);
	chanin = h.nchans;
	// tr - fb
	tr_fb[0] = 0.0f;
	tr_fb[3] = axfreq.front();
	tr_fb[2] = h.tsamp;
	tr_fb[4] = h.foff * h.nchans / chanout;
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
	//static_assert (_offset == 0, "CandidatePlot::ReadFB offset != 0");
	// nsamps
	nsamps = _nsamps;
	// reserve
	__zfill (fb_tshape, nsamps);
	__zfill (fb_fshape, chanout);
	// nsamps
	__zfill (fb, nsamps * chanout);
	// time axis
	__arange (axtime, 0.0f, (float_t)( tsamp ), nsamps);
	// fscrunch
	Fscrunch (f, nsamps, chanout, fb);
	// ABShape
	ABShape (fb, nsamps, chanout, fb_tshape, fb_fshape);
#if 0
{
    std::ofstream ofb ("tfb.dat");
    std::ostream_iterator<float_t> osi(ofb, "\n");
    std::copy (fb.begin(), fb.end(), osi);
    ofb.close();
  }
#endif
}

void cp::ReadBT (const FloatVector_t& f, 
  const Unsigned_t& _nsamps,
  const Unsigned_t& _offset) {
	// offset should always be zero
	//static_assert (_offset == 0, "CandidatePlot::ReadBT offset != 0");
	// XXX It is called after ReadFB has been called
	if (_nsamps != nsamps) {
		std::cerr << "FilterbankBowtiePlot::ReadFB read different." << std::endl;
		nsamps = std::min (nsamps, _nsamps);
	}
	// resize
	__zfill (bt_fshape, axdm.size());
	//__zfill (bt_tshape, nsamps);
	// copy 
	bt = f;
	// ABShape
	//ABMaxShape (bt, axdm.size(), nsamps, bt_fshape, bt_tshape);
	BMaxShape (bt, nsamps, axdm.size(), bt_fshape);
	// for IO
#if 0
{
    std::ofstream ofb ("tbt.dat");
    std::ostream_iterator<float_t> osi(ofb, "\n");
    std::copy (bt.begin(), bt.end(), osi);
    ofb.close();
  }
#endif
}
