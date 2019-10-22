#include "trishul.hpp"
#include "trishul/FilterbankBowtiePlot.hpp"

#include <cstring>
#include <cmath>

using fbt = FilterbankBowtiePlot;

constexpr std::array<float,5> fbt::heat_l;
constexpr std::array<float,5> fbt::heat_r;
constexpr std::array<float,5> fbt::heat_g;
constexpr std::array<float,5> fbt::heat_b;


fbt::FilterbankBowtiePlot (float_t charh_) : charh(charh_) {
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
};

void fbt::Plot (const string_t& filename) {
	if (count++) cpgpage ();
	else {
		cpgbeg (0,filename.c_str(),1,1); 
		cpgsch (charh); 
		cpgask (1);
		cpgpap (0.0, 0.618);
	}
	////////////////////////////////////////////////////////////////
	// DE-DEDISPERSED WATERFALL
	cpgsci(1); // color index
	cpgsvp (0.1, 0.75, 0.1, 0.5);
	cpgswin (
			tleft, tright,
			axfreq.front(), axfreq.back()
	);
	cpgbox ("BCN",0.0,0,"BCNV",0.0,0);
	cpgsfs (1);
	cpgctab (
			heat_l.data(), heat_r.data(), 
			heat_g.data(), heat_b.data(), 
			heat_l.size(), constrast, brightness);
	tr_fb[0] = tleft;
	cpgimag (fb.data(), chanout, nsamps,
			1, chanout, 1, nsamps,
			cmin, cmax, tr_fb
	);
	cpgmtxt("B",2.5,.5,0.5,"Time (s)");
	cpgmtxt("L",4,0.5,0.5,"Freq (MHz)");
	// BANDSHAPE
	cpgsfs(1);
	cpgsci(1); // color index
	cpgsvp(0.75, 0.90, 0.1, 0.5); // bandshape
	__ranger (fb_fshape.cbegin(), fb_fshape.cend(), xxmin, xxmax);
	cpgswin (xxmin, xxmax, axfreq.front(), axfreq.back());
	cpgbox("BCN",0.0,0,"BCV",0.0,0);
	cpgline(chanout, fb_fshape.data(), axfreq.data());
	cpgmtxt("B",2.5,.5,0.5,"Intensity (a.u.)");
	cpgmtxt("T",-1*charh,.5,0.5, "Bandshape");
	// BOWTIE
	cpgsvp (0.1, 0.75, 0.5, 0.9);
	cpgswin (
			tleft, tright,
			axdm.front(), axdm.back()
	);
	cpgbox ("BC",0.0,0,"BCNV",0.0,0);
	cpgsfs (1);
	cpgctab (
			heat_l.data(), heat_r.data(), 
			heat_g.data(), heat_b.data(), 
			heat_l.size(), constrast, brightness);
	tr_bt[0] = tleft;
	__ranger (bt.begin(), bt.end(), xxmin, xxmax);
	cpgimag (bt.data(), axdm.size(), nsamps,
			1, axdm.size(), 1, nsamps, 
			xxmin, xxmax, tr_bt
	);
	//cpgmtxt("B",2.5,.5,0.5,"Time (s)");
	cpgmtxt("L",4,0.5,0.5,"DM (pc/cc)");
	// TITLE
	char txt[256];
	cpgmtxt("T",1.0,0.0,0.0,group);
	snprintf(txt, 256, "Antenna: ea%02d", stationid);
	cpgmtxt("T",1.0,0.5, 0.5, txt);
	snprintf(txt, 256, "Source: %s", name);
	cpgmtxt("T",1.0, 0.9, 1.0, txt);
	// DMAXSHAPE
	cpgsfs(1);
	cpgsci(1); // color index
	cpgsvp(0.75, 0.90, 0.5, 0.9); // bandshape
	__ranger (bt_fshape.cbegin(), bt_fshape.cend(), xxmin, xxmax);
	cpgswin (xxmin, xxmax, axdm.front(), axdm.back());
	cpgbox("BCM",0.0,0,"BCV",0.0,0);
	cpgline(axdm.size(), bt_fshape.data(), axdm.data());
	cpgmtxt("B",2.5,.5,0.5,"Max / DM"); 
	////////////////////////////////////////////////////////////////
	// cpgend deledated to dtor
}

void fbt::ReadFB (const FloatVector_t& f, const Unsigned_t& _nsamps) {
	// nsamps
	nsamps = _nsamps;
	tleft = tright;
	tright = tleft + (nsamps*tsamp);
	// reserve
	__zfill (fb_fshape, chanout);
	// nsamps
	auto fb_size   = nsamps * chanout;
	__zfill (fb, fb_size);
	// fscrunch
	Fscrunch (f, nsamps, fb);
	// ABShape
	BShape (fb, nsamps, chanout, fb_fshape);
}

void fbt::ReadBT (const FloatVector_t& f, const Unsigned_t& _nsamps) {
	// XXX It is called after ReadFB has been called
	if (_nsamps != nsamps) {
		std::cerr << "FilterbankBowtiePlot::ReadFB read different." << std::endl;
		nsamps = std::min (nsamps, _nsamps);
	}
	// reserve
	__zfill (bt_fshape, axdm.size());
	// nsamps
	auto bt_size   = nsamps * axdm.size();
	__zfill (fb, bt_size);
	// fscrunch
	bt = f;
	// ABShape
	BMaxShape (bt, nsamps, axdm.size(), bt_fshape);
}

void fbt::Read (const Header_t& h, const Trigger_t& t) {
	// frequency axis
	__arange (axfreq, (float_t)h.fch1, (float_t)h.foff*h.nchans / chanout, chanout);
	chanin = h.nchans;
	// tr - fb
	tr_fb[0] = 0.0f;
	tr_fb[3] = axfreq.front();
	tr_fb[2] = h.tsamp;
	tr_fb[4] = h.foff * h.nchans / chanout;
	// tr - bt
	tr_bt[0] = 0.0f;
	tr_bt[2] = h.tsamp;
	cmin  = 0;
	cmax  = pow (2, h.nbits) - 1;
	// parameters
	tsamp = h.tsamp;
	stationid = h.stationid;
	strcpy (name, h.name);
	strcpy (group, h.group);
}
