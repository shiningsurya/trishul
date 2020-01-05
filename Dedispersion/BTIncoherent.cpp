#include "trishul.hpp"
#include "trishul/BTIncoherent.hpp"

using bti = BTIncoherent;

void bti::CreatePlan (
		const double_t   _tsamp,
		const unsigned_t _nchans, 
		const float_t    _freq_ch1,
		const float_t    _freq_off
		) {
	// copy parameters
	tsamp    = _tsamp;
	nchans   = _nchans;
	// freq_off is positive
	freq_off = -_freq_off;
	freq_ch1 = _freq_ch1;
}

void bti::Execute (
  const FloatVector_t& in,
  Unsigned_t _nsamps,
  FloatVector_t& out) {

  // this idelays correspond to dm_list
  // idelays.size () == dm_count
  Unsigned_t ddnsamps = _nsamps - idelays.back();
  unsigned_t dm_count = dm_list.size();
  out.resize (dm_count*ddnsamps, 0.0f);

  // for every dm
  for (unsigned_t idm = 0; idm < dm_count; idm++) {
    // this changes idelays
    Delays (dm_list[idm]);
    for (unsigned_t ichan = 0; ichan < nchans; ichan++) {
      Unsigned_t idd = idelays[ichan];
      for (Unsigned_t isamp = 0; isamp < ddnsamps; isamp++) {
        out[isamp +ddnsamps*idm] += in[ichan +nchans*isamp +nchans*idd];
      } // for every sample
    } // for every channel
  } // for every dm
}
