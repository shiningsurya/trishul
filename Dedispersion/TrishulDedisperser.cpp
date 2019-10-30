#include "trishul/Dedisperser.hpp"

using td = TrishulDedisperser;

void td::Delays (float_t _dm) {
	single_dm = _dm;
	float_t if02 = 1.0f/freq_ch1/freq_ch1;
	float_t dd = 0.0f;
	float_t f0 = freq_ch1;
	float_t f1 = 0.0f;
	//
	for (unsigned_t i = 0; i < nchans; i++) {
		f1 = f0 + (i * freq_off);
		dd = 4148.741601 * ((1.0f/f1/f1) - if02) * single_dm; 
		idelays.push_back (dd / tsamp);
	}
} 

float_t td::MaxDM (Unsigned_t nsamps) {
  // max possible dm
	float_t f0 = freq_ch1;
	float_t f1 = f0 + ( nchans * freq_off);

	float_t tdm = 4148.741601 * ((1.0f/f1/f1) - (1.0f/f0/f0));

	return nsamps * tsamp / tdm;
}
