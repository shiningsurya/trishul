#pragma once
#include "trishul.hpp"

class TrishulDedisperser {
  private:
    static inline float_t _single_dm_delay (float_t dm, float_t f0, float_t f1) {
      return 4148.741601 * (1.0/f0/f0 - 1.0/f1/f1) * dm;
    }
	protected:
		// parameters
		double_t           tsamp;
		float_t            freq_ch1; 
		float_t            freq_off;
		unsigned_t         nchans;
		// state
		bool               _is_plan_initialized;
		bool               _is_dmlist_ready;
	public:
		// search parameters
		FloatVector_t      dm_list;
		// single dm
		float_t                 single_dm;
		std::vector<Unsigned_t> idelays;
		virtual void CreatePlan (const double_t, const unsigned_t, const float_t, const float_t) = 0;
		
		virtual void Execute (const FloatVector_t&, Unsigned_t, FloatVector_t&) = 0;
		virtual void Execute (const ByteVector_t&,  Unsigned_t, ByteVector_t&) = 0;

		void Delays (float_t _dm);
		float_t MaxDM (Unsigned_t nsamps);

		void SetDM (const float_t a, const float_t b, const unsigned_t n) noexcept { 
		  // freqs
		  float_t fmax = freq_ch1;
		  float_t fmin = fmax;
		  if (freq_off < 0.0f) fmin += nchans*freq_off;
		  else fmin -= nchans*freq_off;
		  // dms
			float_t start = a;
			float_t step = (b - a) / n;
			// 
			for (unsigned_t i = 0; i < n; i++,start+=step) {
			  dm_list.push_back (start);
			  idelays.push_back ( _single_dm_delay (start, fmin, fmax) /tsamp);
			}
			//
			_is_dmlist_ready = true;
		}

		void SetDM (const FloatVector_t& fv) noexcept {
			dm_list = fv;
		} 
};

