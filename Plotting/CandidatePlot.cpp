#include "trishul.hpp"
#include <cmath>
#include "trishul/CandidatePlot.hpp"

#include "trishul/Globals.hpp"

using cp = CandidatePlot;

cp::CandidatePlot (
  float_t charh_,
  unsigned_t w,
  unsigned_t h
  ) : charh(charh_),
  gr(0, w, h) {
	// __ranger factor
	xxfac = 0.1f;	
	// chanout
	chanout = 512;
	// times
	tleft = tright = 0.0f;
}

void cp::Plot (const string_t& filename) {
	// global properties
  gr.SetScaleText (true);
	////////////////////////////////////////////////////////////////
	// DE-DEDISPERSED WATERFALL
  gr.SubPlot (2,1,0, "<");
  gr.ColumnPlot (2,1);
	gr.SetRanges (tleft, tright, fhigh, flow);
	gr.Box(); gr.Axis();
	//gr.SetRange ('c', cmin, );
	gr.Dens (_fb,"BbcyrR");
	gr.Label ('x', "Time (s)", 0);
	gr.Label ('y', "Freq (MHz)", 0);
	// BOWTIE
	gr.Perspective(0.05);
	gr.ColumnPlot (2,0, 0.2);
	gr.Rotate (15,0);
	gr.SetAutoRanges (tleft, tright, axdm.front(), axdm.back());
	gr.SetRange ('z', _bt);
	gr.SetRange ('c', _bt);
	gr.Surf(_bt, "BbcyrR");
	gr.Perspective(0.0);
	gr.Axis("y");
	gr.Label ('y', "DM (pc/cc)", 0);
	gr.SetTicks ('y', 20, 7, axdm.front()+5);
	// Profile
  gr.SubPlot (2,1,1, "");
  gr.ColumnPlot (2,0);
  gr.SetRange ('x', tleft, tright);
  gr.SetRange ('y', _fb_tshape);
  gr.Plot (_fb_tshape);
	gr.Axis("x");
	// text
	float_t xx = 0.8, yy=1.0, off=0.04;
	snprintf(_fn, sizeof(_fn), "S/N: %3.2f", sn);
	gr.Puts (xx, yy, _fn,":aC", 2.8); yy -= off;
	snprintf(_fn, sizeof(_fn), "DM: %3.2f pc/cc", dm);
	gr.Puts (xx, yy, _fn,":aC", 2.8); yy -= off;
	snprintf(_fn, sizeof(_fn), "Width: %3.2f ms", width*1e3f);
	gr.Puts (xx, yy, _fn,":aC", 2.8); yy -= off;
	snprintf(_fn, sizeof(_fn), "Antenna: ea%02d", stationid);
	gr.Puts (xx, yy, _fn,":aC", 2.8); yy -= off;
	snprintf(_fn, sizeof(_fn), "Source: %s", name);
	gr.Puts (xx, yy, _fn,":aC", 2.8); yy -= off;
	snprintf(_fn, sizeof(_fn), "%s", 
    escape_string(group, sizeof(group)).c_str());
	gr.Puts (0.3, 0.9, _fn,":AC", 2.8);
	////////////////////////////////////////////////////////////////
	snprintf (_fn, sizeof(_fn), "%s.png", filename.c_str());
	gr.WritePNG (_fn);
}

void cp::Read (const Header_t& h, const Trigger_t& t) {
  fhigh = flow = h.fch1; 
  flow += (h.foff * h.nchans);
	chanin = h.nchans;
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
	cmin = 0; cmax = pow(2, nbits) - 1;
}

void cp::ReadFB (const FloatVector_t& f, const Unsigned_t& _nsamps, const Unsigned_t& _offset) {
	// offset should always be zero
	//static_assert (_offset == 0, "CandidatePlot::ReadFB offset != 0");
	// nsamps
	nsamps = _nsamps;
	overlap= _offset;
	tleft = 0.0f;
	tright = tleft + (nsamps*tsamp);
	auto fb_size   = nsamps * chanout;
	if (fb.size() != fb_size)
    __zfill (fb, fb_size);
  if (fb_tshape.size() != nsamps)
    __zfill (fb_tshape, nsamps);
  if (fb_fshape.size() != chanout)
    __zfill (fb_fshape, chanout);
	// fscrunch
	Fscrunch (f, nsamps, chanout, fb);
	// ABShape
	ABShape (fb, nsamps, chanout, fb_tshape, fb_fshape);
	// linking
	_fb.Link (fb.data(), nsamps, chanout);
	_fb.Transpose ();
	_fb_tshape.Link (fb_tshape.data(), nsamps);
	_fb_fshape.Link (fb_fshape.data(), chanout);
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
		std::cerr << "CandidatePlot::ReadBT read different." << std::endl;
		nsamps = std::min (nsamps, _nsamps);
	}
	if (_offset != overlap) {
		std::cerr << "CandidatePlot::ReadBT overlap different." << std::endl;
		overlap = std::min (overlap, _offset);
	}
	// copy 
	bt = f;
  _bt.Link (bt.data(), nsamps, axdm.size());
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
