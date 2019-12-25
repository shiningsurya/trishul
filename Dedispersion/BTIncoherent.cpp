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
	// incoh
	incoh.CreatePlan (tsamp, nchans, freq_ch1, freq_off);
}

void bti::Execute (
  const FloatVector_t& in,
  Unsigned_t _nsamps,
  FloatVector_t& out) {

  Unsigned_t ddnsamps = _nsamps - idelays.back();
  unsigned_t dm_count = dm_list.size();
  out.resize (dm_count*ddnsamps, 0.0f);

  for (unsigned_t idm = 0; idm < dm_count; idm++) {
    FloatVector_t iout;
    Unsigned_t    idd;
    // de-dispersion
    incoh.Delays  (dm_list[idm]);
    idd = _nsamps - incoh.MaxSampDelay ();
    incoh.Execute (in, idd, iout);
    // freq averaging
    // outputing
    for (unsigned_t isamp = 0; isamp < ddnsamps; isamp++)
    for (unsigned_t ichan = 0; ichan < nchans  ; ichan++)
      out[isamp +ddnsamps*idm] += iout[ichan +nchans*isamp]/nchans;
  } // for every dm
}
