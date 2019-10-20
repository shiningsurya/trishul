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
		bool               _is_dmlist_ready;
		// search parameters
		FloatVector_t      dm_list;
		// single dm
		float_t                 single_dm;
		std::vector<Unsigned_t> idelays;
	public:
		virtual void CreatePlan (const double_t, const unsigned_t, const float_t, const float_t) = 0;
		
		virtual void Execute (const FloatVector_t&, Unsigned_t, FloatVector_t&) = 0;
		virtual void Execute (const ByteVector_t&,  Unsigned_t, ByteVector_t&) = 0;

		void Delays (float_t _dm);

		void SetDM (const float_t a, const float_t b, const unsigned_t n) noexcept { 
			float_t start = a;
			float_t step = (b - a) / n;
			// 
			dm_list.push_back (start);
			for (unsigned_t i = 1; i < n; i++,start+=step) dm_list.push_back (start);
			//
			_is_dmlist_ready = false;
		}

		void SetDM (const FloatVector_t& fv) noexcept {
			dm_list = fv;
		} 
};

struct TrishulDedisperserError : std::exception {
  char * msg;
  TrishulDedisperserError (const char * _msg) {
    strcpy (msg, _msg);
  }
  const char * what() const noexcept { return msg; }
};
