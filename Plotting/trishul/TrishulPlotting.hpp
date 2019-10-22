#pragma once
#include "trishul.hpp"
#include "trishul/Header.hpp"

class TrishulPlotting {
	public:
		static unsigned_t       chanin;
		static unsigned_t       chanout;

		Unsigned_t       nsamps;

		static float_t   xxfac;
		
		static void Fscrunch (const FloatVector_t& fin, Unsigned_t& nsamps, FloatVector_t& fout);
		static void ABShape (const FloatVector_t& f, const Unsigned_t& nsamps, const unsigned_t& nchans, FloatVector_t& tshape, FloatVector_t& bshape);
		static void BShape (const FloatVector_t& f, const Unsigned_t& nsamps, const unsigned_t& nchans, FloatVector_t& bshape);
		static void BMaxShape (const FloatVector_t& f, const Unsigned_t& nsamps, const unsigned_t& nchans, FloatVector_t& bshape);

		static void __ranger (FloatVector_t::const_iterator i, FloatVector_t::const_iterator j, 
				float_t& xxmin,
				float_t& xxmax);
		static void __arange (FloatVector_t& ptr, float_t start, float_t step, Unsigned_t size);
		static void __zfill (FloatVector_t& ptr, Unsigned_t size);
	public:

		virtual void Read (const Header_t& h, const Trigger_t& t) = 0;
		virtual void ReadFB (const FloatVector_t& f, const Unsigned_t& nsamps) = 0;
		virtual void ReadBT (const FloatVector_t& f, const Unsigned_t& nsamps) = 0;

		virtual void Plot (const string_t& filename) = 0;
};
