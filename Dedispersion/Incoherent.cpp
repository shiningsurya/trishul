#include "trishul.hpp"
#include "trishul/Incoherent.hpp"

void Incoherent::CreatePlan (
		const double_t   _tsamp, 
		const unsigned_t _nchans, 
		const float_t    _fch1, 
		const float_t    _foff
		) {
	// copy parameters
	tsamp    = _tsamp;
	nchans   = _nchans;
	freq_ch1 = _fch1;
	freq_off = _foff;
}

void Incoherent::Execute (
		PtrFloat_t f, Unsigned_t nsamps,
		PtrFloat_t o
	) {
	Unsigned_t idx = 0;
	// This is for single DM case ONLY!
	for (Unsigned_t i = 0; i < nsamps; i++) {
		for (unsigned_t j = 0; j < nchans; j++) {
			idx = nchans * (i + idelays[j]) + j;
			o[nchans*i + j] = f[idx];
		}
	}
}

void Incoherent::Execute (
		PtrByte_t f, Unsigned_t nsamps,
		PtrByte_t o
	) {}
