#pragma once
#include "trishul.hpp"
#include "trishul/TrishulPlotting.hpp"
// pgplot
#include "cpgplot.h"

class CandidateProfilePlot : protected TrishulPlotting {
	private:
		// viewports
		float_t      xmin;
		float_t      xmax;
		float_t      ymin;
		float_t      ymax;
		float_t      charh;
		// images
		float_t      tr[6];
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
		FloatVector_t   fb_tshape;
		FloatVector_t   fb_fshape;
		FloatVector_t   fb;
		Unsigned_t      fb_size;
		FloatVector_t   axtime;
		FloatVector_t   axfreq;
		// parameters
		float_t      sn;
		float_t      dm;
		float_t      width;
		float_t      peak_time;
		float_t      tstart;
		double_t     tsamp;
		unsigned_t   stationid;
		char         name[16];
		char         group[16];
		float_t      duration;
		unsigned_t   nbits;
		Unsigned_t   nsamps;
	public:
		CandidateProfilePlot (float_t _charh = 0.65);
		~CandidateProfilePlot () = default;

		void Read (const Header_t& h, const Trigger_t& t) override;
		void ReadFB (const FloatVector_t& f, const Unsigned_t& nsamps, const Unsigned_t& offset = 0) override;
		void ReadBT (const FloatVector_t& f, const Unsigned_t& nsamps, const Unsigned_t& offset = 0) {
			throw TrishulError("CandidateProfilePlot::ReadBT not supported");
		}

		void Plot (const string_t& filename) override;
};

/*
 *template<typename It>
 *void TrishulPlotting::__ranger (It, It, typename It::reference, typename It::reference);
 *template<typename T>
 *void TrishulPlotting::__arange (std::vector<T>&, T , T , Unsigned_t );
 *template<typename T>
 *void TrishulPlotting::__zfill (std::vector<T>& , Unsigned_t );
 */
