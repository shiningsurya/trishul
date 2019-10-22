#pragma once
#include "trishul.hpp"
#include "trishul/Header.hpp"


class TrishulFormats {
	protected:
		// static counters
		static Unsigned_t h_readtimes;
		static Unsigned_t h_writetimes;
		static Unsigned_t d_readtimes;
		static Unsigned_t d_writetimes;
	public:
		virtual bool ReadHeader (Header_t& , Trigger_t&) = 0;
		virtual bool WriteHeader (const Header_t&, const Trigger_t&) = 0;

		virtual Unsigned_t ReadData  (ByteVector_t&, Unsigned_t) = 0;
		virtual Unsigned_t WriteData (ByteVector_t&, Unsigned_t) = 0;
};

