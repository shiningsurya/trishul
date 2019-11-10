#include "trishul.hpp"
#include "trishul/FilterbankBowtiePlot.hpp"

#include <cstring>
#include <cmath>


using fbt = FilterbankBowtiePlot;

fbt::FilterbankBowtiePlot (float_t charh_, unsigned_t w, unsigned_t h) : 
charh(charh_),
gr (0, w, h) {
	// __ranger factor
	xxfac = 0.1f;	
	// chanout
	chanout = 512;
	// count
	count = 0;
	// times
	tleft = tright = 0.0f;
}

void fbt::Plot (const string_t& filename) {
  // new frame
  if (count) gr.NewFrame ();
	// global properties
  gr.SetScaleText (true);
  gr.SubPlot (1,1,0, "");
	////////////////////////////////////////////////////////////////
	// DE-DEDISPERSED WATERFALL
	gr.ColumnPlot (2,1);
	gr.SetRanges (tleft, tright, fhigh, flow);
	gr.Box(); gr.Axis();
	gr.Dens (_fb);
	gr.Label ('x', "Time (s)", 0);
	gr.Label ('y', "Freq (MHz)", 0);
	// BOWTIE
	gr.Perspective(0.05);
	gr.ColumnPlot (2,0, 0.2);
	gr.Rotate (15,0);
	gr.SetRanges (tleft, tright, axdm.front(), axdm.back());
	gr.SetRange ('z', _bt);
	gr.Surf(_bt, "BbcyrR");
	gr.Perspective(0.0);
	gr.Axis("y");
	gr.Label ('y', "DM (pc/cc)", 0);
	gr.SetTicks ('y', 20, 7, 10);
	// TITLE
	gr.Puts (0.2, .9, group);
	snprintf(_fn, sizeof(_fn), "Antenna: ea%02d", stationid);
	gr.Puts (0.5, .9, _fn);
	snprintf(_fn, sizeof(_fn), "Source: %s", name);
	gr.Puts (0.8, 0.9, _fn);
	////////////////////////////////////////////////////////////////
	snprintf (_fn, sizeof(_fn), "%s_%04d.png", filename.c_str(), count);
	gr.WritePNG (_fn);
	gr.EndFrame ();
	count++;
}

void fbt::ReadFB (const FloatVector_t& f, const Unsigned_t& _nsamps, const Unsigned_t& _offset) {
	// nsamps
	nsamps = _nsamps;
	overlap= _offset;
	tleft = tright - (_offset*tsamp);
	tleft = tleft >= 0.0f ? tleft : 0.0f;
	tright = tleft + (nsamps*tsamp);
	//v --ordering is important
	//tleft = tright - (_offset*tsamp);
	//tright = tleft + (nsamps*tsamp);
	// reserve
	// nsamps
	auto fb_size   = nsamps * chanout;
	std::cout << nsamps << std::endl;
	std::cout << chanout<< std::endl;
	std::cout << fb_size<< std::endl;
	std::cout << fb.size()<< std::endl;
	if (fb.size() != fb_size)
    __zfill (fb, fb_size);
	// fscrunch
	Fscrunch (f, nsamps, chanout, fb);
	// linking
  _fb.Link (fb.data(), nsamps, chanout);
  _fb.Transpose();
}

void fbt::ReadBT (const FloatVector_t& f, 
  const Unsigned_t& _nsamps,
  const Unsigned_t& _offset) {
	// XXX It is called after ReadFB has been called
	if (_nsamps != nsamps) {
		std::cerr << "FilterbankBowtiePlot::ReadFB read different." << std::endl;
		nsamps = std::min (nsamps, _nsamps);
	}
	if (_offset != overlap) {
		std::cerr << "FilterbankBowtiePlot::ReadFB read different." << std::endl;
		overlap = std::min (overlap, _offset);
	}
	// copy 
	bt = f;
  _bt.Link (bt.data(), nsamps, axdm.size());
}

void fbt::Read (const Header_t& h, const Trigger_t& t) {
  fhigh = flow = h.fch1; 
  flow += (h.foff * h.nchans);
	chanin = h.nchans;
	// parameters
	tsamp = h.tsamp;
	stationid = h.stationid;
	strcpy (name, h.name);
	strcpy (group, h.group);
}
