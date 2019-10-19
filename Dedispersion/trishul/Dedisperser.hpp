#pragma once
#include "trishul.hpp"

class TrishulDedisperser {
	protected:
		// parameters
		double_t           tsamp;
		float_t            freq_ch1; 
		float_t            freq_off;
		unsigned_t         nchans;
		// state
		bool               _is_plan_initialized;
		// search parameters
		float_t            dm_low;
		float_t            dm_high;
		PtrFloat_t         dm_list;
		unsigned_t         dm_count;
		// single dm
		float_t                 single_dm;
		std::vector<Unsigned_t> idelays;
	public:
		virtual void CreatePlan (const double_t, const unsigned_t, const float_t, const float_t) = 0;
		
		virtual void Execute (PtrFloat_t, Unsigned_t, PtrFloat_t) = 0;
		virtual void Execute (PtrByte_t,  Unsigned_t, PtrByte_t) = 0;

		void Delays (float_t _dm);

		void SetDM (const float_t a, const float_t b) noexcept { 
			dm_low = a; dm_high = b;
		}

		void SetDM (const FloatVector_t& fv) noexcept {
			dm_list = new float[fv.size()];
			std::copy (fv.begin(), fv.end(), dm_list);
		} 
};
