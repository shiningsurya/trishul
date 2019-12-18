#pragma once
#include "trishul.hpp"
#include "trishul/TrishulPlotting.hpp"

// mathgl
#include <mgl2/mgl.h>

class BowtiePlot : protected TrishulPlotting {
		// viewports
		char         _fn[256];
		mglGraph     gr;
		// images
		unsigned_t   txtrow;
		unsigned_t   cmin;
		unsigned_t   cmax;
		// parameters
		float_t      sn;
		float_t      dm;
		float_t      width;
		float_t      peak_time;
		float_t      tstart;
		double_t     tsamp;
		unsigned_t   stationid;
		char         name[16];
		char         group[21];
		float_t      duration;
		unsigned_t   nbits;
		Unsigned_t   nsamps;
		// ranger
		float_t      xxfac;
		float_t      xxmin;
		float_t      xxmax;
		float_t      dd_range;
		// plot
		FloatVector_t   bt;
		mglData         _bt;
		float_t         tleft;
		float_t         tright;
		FloatVector_t   axdm;
		float_t         flow;
		float_t         fhigh;
  public:
    BowtiePlot (unsigned_t _w = 1200, unsigned_t _h = 900);
    ~BowtiePlot () = default;

		void Read (const Header_t& h, const Trigger_t& t) override;
		void ReadFB (const FloatVector_t& f, const Unsigned_t& nsamps, const Unsigned_t& offset = 0) {
		  throw TrishulError ("This ain't gonna work.");
		}
		void ReadBT (const FloatVector_t& f, const Unsigned_t& nsamps, const Unsigned_t& offset = 0) override;

		void SetDM (const FloatVector_t& dms) { 
			axdm = dms;
		}

		void Plot (const string_t& filename) override;
};
