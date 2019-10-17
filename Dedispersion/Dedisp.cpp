#include "trishul/Dedisperser.hpp"

Dedisp::Dedisp () {}

void Dedisp::CreatePlan (
		const double_t   _tsamp,
		const unsigned_t _nchans, 
		const float_t    _freq_ch1,
		const float_t    _freq_off
		) override final {
	// copy parameters
	tsamp    = _tsamp;
	nchans   = _nchans;
	freq_ch1 = _freq_ch1;
	freq_off = _freq_off;

	// create plan
	error = dedisp_create_plan (
			&dplan, nchans, tsamp,
			freq_ch1, freq_off
	);
	if (error != DEDISP_NO_ERROR ) 
		throw TrishulDedispersionError("Unable to create plan.");
}

template<typename T, typename U>
void Dedisp::Execute (
		T* in, unsigned_t nsamps, U* out
		) override final {
	// executing
	error = dedisp_execute (
			dplan, (dedisp_size) nsamps, 
			(const dedisp_byte*) in, sizeof(T) * 8,
			(dedisp_byte*) out     , sizeof(U) * 8,
			DEDISP_USE_DEFAULT
	);
	if (error != DEDISP_NO_ERROR) 
		throw new TrishulDedispersionError(dedisp_get_error_string(error));

}
