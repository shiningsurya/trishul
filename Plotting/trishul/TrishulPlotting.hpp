#pragma once
#include "trishul.hpp"
#include "trishul/Header.hpp"

class TrishulPlotting {
	protected:
		unsigned_t       chanin;
		unsigned_t       chanout;

		Unsigned_t       nsamps;
		
		void Fscrunch (PtrFloat_t fin, Unsigned_t& nsamps, PtrFloat_t fout);
		void ABShape (PtrFloat_t f, const Unsigned_t& nsamps, const unsigned_t& nchans, PtrFloat_t tshape, PtrFloat_t bshape);
	public:

		virtual void Read (const Header_t& h, const Trigger_t& t) = 0;
		virtual void Read (PtrFloat_t f, const Unsigned_t& nsamps) = 0;

		virtual void Plot (const string_t& filename) = 0;
};
