#include "trishul/Dedisp.hpp"

Dedisp::Dedisp () = default;

Unsigned_t Dedisp::MaxSampDelay () noexcept { 
  if (!_is_dmlist_ready) {
    dedisp_set_dm_list (dplan, dm_list.data(), dm_list.size());
    _is_dmlist_ready = true;
  }
  max_delay = dedisp_get_max_delay (dplan);
  return max_delay; 
}

float_t Dedisp::MaxTimeDelay () noexcept { 
  if (!_is_dmlist_ready) {
    dedisp_set_dm_list (dplan, dm_list.data(), dm_list.size());
    _is_dmlist_ready = true;
  }
  max_delay = dedisp_get_max_delay (dplan);
  return max_delay*tsamp; 
}

void Dedisp::CreatePlan (
		const double_t   _tsamp,
		const unsigned_t _nchans, 
		const float_t    _freq_ch1,
		const float_t    _freq_off
		) {
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
		throw TrishulError("Unable to create plan.");
	// set state
	_is_plan_initialized = true;
}

void Dedisp::Execute (
  const FloatVector_t& in,
  Unsigned_t nsamps,
  FloatVector_t& out) {
  // dedisp is C and has no support for vector
  // and we don't want to have raw pointers
  // so we play a trick here
  // we prefill out(FloatVector_t) to the required size
  // and pass data() to dedisp
  Unsigned_t ddnsamps = nsamps - MaxSampDelay();

  // Let caller take care of sizes
  // out.resize ( ddnsamps * dm_list.size(), 0.0f );
  // executing
	error = dedisp_execute (
			dplan, (dedisp_size) nsamps, 
			(const dedisp_byte*) in.data(), sizeof(float_t) * 8,
			(dedisp_byte*) out.data()     , sizeof(float_t) * 8,
			DEDISP_USE_DEFAULT
	);
	if (error != DEDISP_NO_ERROR ) 
		throw TrishulError("Failed to execute!.");
}

void Dedisp::Execute (
  const ByteVector_t& in,
  Unsigned_t nsamps,
  ByteVector_t& out) {
  // dedisp is C and has no support for vector
  // and we don't want to have raw pointers
  // so we play a trick here
  // we prefill out(FloatVector_t) to the required size
  // and pass data() to dedisp
  Unsigned_t ddnsamps = nsamps - MaxSampDelay();
  // Let caller take care of sizes
  // out.resize ( ddnsamps * dm_list.size(), 0.0f );
  // executing
	error = dedisp_execute (
			dplan, (dedisp_size) nsamps, 
			(const dedisp_byte*) in.data(), sizeof(Byte_t) * 8,
			(dedisp_byte*) out.data()     , sizeof(Byte_t) * 8,
			DEDISP_USE_DEFAULT
	);
	if (error != DEDISP_NO_ERROR ) 
		throw TrishulError("Failed to execute!.");
}

void Dedisp::Execute (
  const ByteVector_t& in,
  unsigned_t nbits,
  Unsigned_t nsamps,
  FloatVector_t& out) {
  // dedisp is C and has no support for vector
  // and we don't want to have raw pointers
  // so we play a trick here
  // we prefill out(FloatVector_t) to the required size
  // and pass data() to dedisp
  // Let caller take care of sizes
  // out.resize ( ddnsamps * dm_list.size(), 0.0f );
  
  // executing
	error = dedisp_execute (
			dplan, (dedisp_size) nsamps, 
			(const dedisp_byte*) in.data(), nbits,
			(dedisp_byte*) out.data()     , sizeof(float_t) * 8,
			DEDISP_USE_DEFAULT
	);
	if (error != DEDISP_NO_ERROR ) 
		throw TrishulError("Failed to execute!.");
}
