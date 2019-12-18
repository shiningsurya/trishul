#include "trishul.hpp"
#include <cmath>
#include "trishul/BowtiePlot.hpp"

#include "trishul/Globals.hpp"

using bp = BowtiePlot;

bp::BowtiePlot (
  unsigned_t w,
  unsigned_t h
  ) : 
  gr(0, w, h) {
	// __ranger factor
	xxfac = 0.1f;	
	// times
	tleft = tright = 0.0f;
}

void bp::Read (const Header_t& h, const Trigger_t& t) {
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

void bp::Plot (const string_t& filename) {
	// global properties
  gr.SetScaleText (true);
	////////////////////////////////////////////////////////////////
	// BOWTIE
	gr.Perspective(0.05);
	gr.ColumnPlot (1,0);
	gr.Rotate (15,0);
	gr.SetAutoRanges (tleft, tright, axdm.front(), axdm.back());
	gr.SetRange ('z', _bt);
	gr.SetRange ('c', _bt);
	gr.Surf(_bt, "BbcyrR");
	gr.Perspective(0.0);
	gr.Axis("y");
	gr.Label ('y', "DM (pc/cc)", 0);
	gr.Label ('x', "Time (s)", 0);
	gr.SetTicks ('y', 20, 7, axdm.front()+5);
	////////////////////////////////////////////////////////////////
	snprintf (_fn, sizeof(_fn), "%s.png", filename.c_str());
	gr.WritePNG (_fn);
}

void bp::ReadBT (const FloatVector_t& f, 
  const Unsigned_t& _nsamps,
  const Unsigned_t& _offset) {
	// offset should always be zero
	//static_assert (_offset == 0, "CandidatePlot::ReadBT offset != 0");
	// XXX It is called after ReadFB has been called
	nsamps = _nsamps;
	overlap = _offset;
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
