#pragma once
#include "trishul.hpp"
#include "trishul/Dedisperser.hpp"

#include <cmath>

class FDMT_CPU : public TrishulDedisperser {
	private:
		// Main state	
		Unsigned_t      nsamps;
		FloatVector_t   State;
		// strides for state
		Unsigned_t      salpha;
		Unsigned_t      sbeta;
		// DM ranges
		Unsigned_t      aDT;
		Unsigned_t      bDT;
		Unsigned_t      maxDT;
		// Freq
		float_t         fmin;
		float_t         fmax;
		unsigned_t      l2nch;
		//
		void Initialization (const FloatVector_t& );
		void Iteration (const unsigned_t& );
		static inline float_t cff (float_t f0, float_t f1, float_t f2) {
				return (std::pow(f0,-2) - std::pow(f1,-2))/(std::pow(f0,-2) - std::pow(f2,-2));
		}
		static inline float_t dff (float_t f0, float_t f1) {
				return std::pow (f0,-2) - std::pow (f1,-2);
		}
	public:
		FDMT_CPU ()  = default;
		~FDMT_CPU () = default;

		void CreatePlan (const double_t, const unsigned_t, const float_t, const float_t) override;

		void Execute (const FloatVector_t&, Unsigned_t, FloatVector_t&) override;
		void Execute (const ByteVector_t&,  Unsigned_t, ByteVector_t& ) override {
			throw TrishulError ("Not available for FDMT_CPU.");
		}

		Unsigned_t MaxSampDelay () noexcept;
};
