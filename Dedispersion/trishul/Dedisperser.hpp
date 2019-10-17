#include "trishul.hpp"

class TrishulDedisperser {
	private:
		// parameters
		double_t           tsamp;
		float_t            freq_ch1; 
		float_t            freq_off;
		unsigned_t         nchans;
		// state
		bool               _is_plan_initialized;
		TrishulVerbosity   verbose;
		// search parameters
		float_t            dm_low;
		float_t            dm_high;
		PtrFloat_t         dm_list;
		unsigned_t         dm_count;
		PtrUnsigned_t      kill_mask;
	public:
		virtual CreatePlan (const double_t, const unsigned_t, const float_t, const float_t);
		
		virtual Execute ();

		void SetDM (const float_t a, const float_t b) noexcept { 
			dm_low = a; dm_high = b;
		}

		void SetDM (const FloatVector_t& fv) noexcept {
			dm_list = new float[fv.size()];
			std::copy (fv.begin(), fv.end(), dm_list);
		} 

		template<typename T>
		void SetKillMask (const std::vector<T>& v) {
			if (v.size() != nchans) {
				if (verbose >= TrishulVerbosity::Warnings) {
					std::cerr << "Trishul::Dedisperser::SetKillMask killmask size != nchan!" << std::endl;
				}
			}
			kill_mask = new unsigned int[nchans];
			std::copy (v.begin(), v.begin() + nchans, kill_mask);
		}
};
