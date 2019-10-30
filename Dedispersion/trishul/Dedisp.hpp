#pragma once
#include "trishul.hpp"
#include "trishul/Dedisperser.hpp"
#include "dedisp.h"

class Dedisp : public TrishulDedisperser {
	private:
		// dedisp
		dedisp_plan        dplan;
		dedisp_error       error;
		Unsigned_t         max_delay;
	public:
		Dedisp () ;
		~Dedisp () {
			if (_is_plan_initialized)
				dedisp_destroy_plan (dplan);
		}

		void CreatePlan (const double_t, const unsigned_t, const float_t, const float_t) override;

		void Execute (const FloatVector_t&, Unsigned_t, FloatVector_t&) override;
		void Execute (const ByteVector_t&,  Unsigned_t, ByteVector_t& ) override;
		void Execute (const ByteVector_t&, unsigned_t, Unsigned_t, FloatVector_t&);

		Unsigned_t MaxSampDelay () noexcept;
		float_t    MaxTimeDelay () noexcept;

};
