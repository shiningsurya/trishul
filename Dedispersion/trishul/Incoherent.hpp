#pragma once
#include "trishul.hpp"
#include "trishul/Dedisperser.hpp"

class Incoherent : public TrishulDedisperser {
	protected:


	public:
		void CreatePlan (const double_t, const unsigned_t, const float_t, const float_t) override;

		void Execute (const FloatVector_t&, Unsigned_t, FloatVector_t&) override;
		void Execute (const ByteVector_t&,  Unsigned_t, ByteVector_t& ) override;

		Unsigned_t MaxSampDelay () const { return idelays.back(); }
		float_t MaxTimeDelay () const { return idelays.back()*tsamp;}
};
