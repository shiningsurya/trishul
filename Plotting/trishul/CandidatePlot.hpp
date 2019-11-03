#pragma once
#include "trishul.hpp"
#include "trishul/TrishulPlotting.hpp"
// pgplot
#include "cpgplot.h"

class CandidatePlot : protected TrishulPlotting {
	private:
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
		constexpr static std::array<float,10> anrb_l = { 0.0, 0.035, 0.045, 0.225, 0.4, 0.41, 0.6, 0.775, 0.985, 1.0 };
		constexpr static std::array<float,10> anrb_r = { 1.0, 1.0, 0.947, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
		constexpr static std::array<float,10> anrb_g = { 1.0, 0.844, 0.8, 0.0, 0.946, 1.0, 1.0, 1.0, 0.0, 0.0 };
		constexpr static std::array<float,10> anrb_b = { 1.0, 1.0, 1.0, 1.0, 1.0, 0.95, 0.0, 0.0, 0.0, 0.0 };
		constexpr static float_t constrast = 1;
		constexpr static float_t brightness = 0.5;
		// ranger
		float_t      xxfac;
		float_t      xxmin;
		float_t      xxmax;
		float_t      dd_range;
		// plot
		FloatVector_t   fb_tshape;
		FloatVector_t   fb_fshape;
		FloatVector_t   fb;
		FloatVector_t   bt_tshape;
		FloatVector_t   bt_fshape;
		FloatVector_t   bt;
		float_t         tleft;
		float_t         tright;
		FloatVector_t   axfreq;
		FloatVector_t   axdm;

	public:
		CandidatePlot (float_t _charh = 0.65);
		~CandidatePlot () = default;

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