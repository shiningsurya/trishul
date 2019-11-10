#pragma once
#include "trishul.hpp"
#include "trishul/TrishulPlotting.hpp"

// mathgl
#include <mgl2/mgl.h>

class FilterbankBowtiePlot : protected TrishulPlotting {
	private:
		// plot
		float_t      charh;
		unsigned_t   count;
		char         _fn[256];
		mglGraph     gr;
		unsigned_t   txtrow;
		// plot
		FloatVector_t   fb;
		mglData         _fb;
		FloatVector_t   bt;
		mglData         _bt;
		float_t         tleft;
		float_t         tright;
		FloatVector_t   axdm;
		float_t         flow;
		float_t         fhigh;
		// parameters
		float_t      tsamp;
		unsigned_t   stationid;
		char         name[16];
		char         group[16];
	public:
		FilterbankBowtiePlot (float_t _charh = 0.65, unsigned_t _w= 1200, unsigned_t _h= 900);
		~FilterbankBowtiePlot () = default;

		void Read (const Header_t& h, const Trigger_t& t) override;
		void ReadFB (const FloatVector_t& f, const Unsigned_t& nsamps, const Unsigned_t& offset = 0) override;
		void ReadBT (const FloatVector_t& f, const Unsigned_t& nsamps, const Unsigned_t& offset = 0) override;

		void SetDM (const FloatVector_t& dms) { 
		  std::cout << "dms.size=" << dms.size() << std::endl;
			axdm = dms;
		}

		void Plot (const string_t& filename) override;
};
