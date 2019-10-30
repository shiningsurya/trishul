#pragma once
#include "trishul.hpp"
#include "trishul/TrishulPlotting.hpp"
// pgplot
#include "cpgplot.h"


class FilterbankBowtiePlot : protected TrishulPlotting {
	private:
		// viewports
		float_t      charh;
		unsigned_t   count;
		// images
		float_t      tr_fb[6];
		float_t      tr_bt[6];
		unsigned_t   txtrow;
		unsigned_t   csize;
		unsigned_t   cmin;
		unsigned_t   cmax;
		// cmaps
		constexpr static std::array<float,5> heat_l = {0.0, 0.2, 0.4, 0.6, 1.0} ;
		constexpr static std::array<float,5> heat_r = {0.0, 0.5, 1.0, 1.0, 1.0} ;
		constexpr static std::array<float,5> heat_g = {0.0, 0.0, 0.5, 1.0, 1.0} ;
		constexpr static std::array<float,5> heat_b = {0.0, 0.0, 0.0, 0.3, 1.0} ;
		// cmap
		constexpr static std::array<float,6> dync_l = {0.0, 0.2, 0.4, 0.6, 0.8, 1.0} ;
		constexpr static std::array<float,6> dync_r = {0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f} ;
		constexpr static std::array<float,6> dync_g = {0.0f, 0.5f, 0.0f, 0.5f, 1.0f, 1.0f} ;
		constexpr static std::array<float,6> dync_b = {0.0f, 0.5f, 0.0f, 0.0f, 0.3f, 1.0f} ;
		constexpr static float_t constrast = 1;
		constexpr static float_t brightness = 0.5;
		// ranger
		float_t      xxfac;
		float_t      xxmin;
		float_t      xxmax;
		float_t      dd_range;
		// plot
		FloatVector_t   fb_fshape;
		FloatVector_t   fb;
		FloatVector_t   bt_fshape;
		FloatVector_t   bt;
		float_t         tleft;
		float_t         tright;
		FloatVector_t   axfreq;
		FloatVector_t   axdm;
		// parameters
		float_t      tsamp;
		unsigned_t   stationid;
		char         name[16];
		char         group[16];
	public:
		FilterbankBowtiePlot (float_t _charh = 0.65);
		~FilterbankBowtiePlot () { if (count) cpgend (); }

		void Read (const Header_t& h, const Trigger_t& t) override;
		void ReadFB (const FloatVector_t& f, const Unsigned_t& nsamps, const Unsigned_t& offset = 0) override;
		void ReadBT (const FloatVector_t& f, const Unsigned_t& nsamps, const Unsigned_t& offset = 0) override;

		void SetDM (const FloatVector_t& dms) { 
			axdm = dms;
			tr_bt[3] = axdm.front();
			tr_bt[4] = (axdm.back() - axdm.front()) / axdm.size();
		}


		void Plot (const string_t& filename) override;
};
