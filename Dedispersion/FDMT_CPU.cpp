#include "trishul.hpp"
#include "trishul/FDMT_CPU.hpp"

using fc = FDMT_CPU;

// Reference ordering   = (F,dT,T)
// Implemented ordering = (T,F,dT)

void fc::CreatePlan (
		const double_t   _tsamp,
		const unsigned_t _nchans, 
		const float_t    _freq_ch1,
		const float_t    _freq_off
		) {
	// copy parameters
	tsamp    = _tsamp;
	nchans   = _nchans;
	l2nch    = log2 (nchans);
	freq_ch1 = _freq_ch1;
	fmax     = _freq_ch1;
	freq_off = _freq_off;
	fmin     = fmax - (nchans*freq_off);
}

Unsigned_t fc::MaxSampDelay () noexcept {
	bDT   =  4148.741601 * dff (fmin, fmax) * dm_list.back () / tsamp;
	return bDT;
}

void fc::Execute (
  const FloatVector_t& in,
  Unsigned_t _nsamps,
  FloatVector_t& out) {
  nsamps = _nsamps;

  // FDMT start
  Initialization (in);
  for (unsigned_t i = 1; i <= l2nch; i++) {
  	Iteration (i);
  }
  // FDMT end
  
  // slicing State(T,1,aDT:bDT) = State(T,aDT:bDT)
  // out is (dT,T)
  #if 1 
  auto ddnsamps = nsamps-bDT;
  out.resize (idelays.size() * ddnsamps);
	for (Unsigned_t idt = 0  ; idt<idelays.size(); idt++)
	for (Unsigned_t isamp = 0; isamp <ddnsamps	 ; isamp++)
		out [isamp +ddnsamps*idt] = State [idelays[idt] +sbeta*isamp];
	#else
	out = State;
	#endif
}

void fc::Iteration (const unsigned_t& i) {
	float_t deltaF    = std::pow (2, i) * freq_off;
	unsigned_t fjumps = nchans / std::pow (2,i);
	float_t    corr   = i > 0 ? 0.5 * freq_off : 0.0f;

	Unsigned_t deltaT = std::ceil ( 
			(maxDT-1)                 *
			dff (fmin+deltaF, fmin)   /
			dff (fmax, fmin)
	);

	FloatVector_t Output (fjumps*nsamps*(deltaT+1));
	Unsigned_t alpha = deltaT+1;
	Unsigned_t beta  = alpha*fjumps;

	for (unsigned_t iF = 0; iF < fjumps; iF++) {
		float_t fstart = fmax - ( deltaF*iF );
		float_t fend   = fmax - ( deltaF*( iF+1 ) );
		float_t fmidl  = 0.5*(fend + fstart) - corr;
		float_t fmid   = 0.5*(fend + fstart) + corr;

		Unsigned_t l_dT = std::ceil (
				(maxDT-1)         *
				dff(fstart,fend)  /
				dff(fmax,  fmin)
		);
		for (Unsigned_t idt = 0; idt <= l_dT; idt++) {
			Unsigned_t midt = idt * dff (fstart, fmid)  / dff (fstart, fend);
			Unsigned_t midtl= idt * dff (fstart, fmidl) / dff (fstart, fend);
			Unsigned_t bp   = nsamps - midtl;
			// 
			for (Unsigned_t itt = bp; itt <nsamps; itt++) 
			//Output.at (iF, idt, itt) = State.at (2*iF, midt, itt);
			Output [idt  +alpha*iF    + beta*itt]   =
			State  [midt +salpha*iF*2 + sbeta*itt]  ;
			// 
			for (Unsigned_t itt = 0; itt <bp; itt++)
			//Output.at (iF,idt, itt) = State.at (2*iF   , midt, itt) + State.at (2*iF +1, idt-midtl, itt+midtl);
			{
			Output [idt       +alpha*iF             +beta*itt]                =
			State  [midt      +2*salpha*iF          +sbeta*itt]               +
			State  [idt-midtl +2*salpha*iF  +salpha +sbeta*itt +sbeta*midtl]  ;
			}
		}
	}

	State  = std::move (Output);
	salpha = alpha;
	sbeta  = beta;
}

void fc::Initialization (const FloatVector_t& Image) {
	aDT   =  4148.741601 * dff (fmin, fmax) * dm_list.front () / tsamp;
	bDT   =  4148.741601 * dff (fmin, fmax) * dm_list.back  () / tsamp;
	maxDT = std::min(bDT, static_cast<Unsigned_t>(nsamps));
	Unsigned_t deltaT = std::ceil ( 
			(maxDT-1)                 *
			dff (fmin, fmin+freq_off) /
			dff (fmin, fmax)
	);

	FloatVector_t Output (Image.size() * (deltaT+1));
	Unsigned_t alpha = deltaT+1;
	Unsigned_t beta  = alpha*nchans;
	for (unsigned_t isamp = 0; isamp < nsamps; isamp++)
	for (unsigned_t ichan = 0; ichan < nchans; ichan++)
		// Output.at (isamp, ichan, 0) = Image.at (isamp, ichan)
		Output [alpha*ichan +beta*isamp] =
		Image  [ichan +nchans*isamp];

	for (Unsigned_t idt = 1; idt <= deltaT; idt++) 
	for (unsigned_t isamp = idt; isamp < nsamps; isamp++)
	for (unsigned_t ichan = 0; ichan < nchans; ichan++)
		//Output.at (isamp, ichan, idt) = Output.at (isamp, ichan, idt-1) + Image.at  (nsamps-isamp+1, ichan);
		Output [idt   +alpha*ichan +beta*isamp] =
		Output [idt-1 +alpha*ichan +beta*isamp] +
		Image  [ichan +nchans*(nsamps-isamp)];

	State  = std::move (Output);
	salpha = alpha;
	sbeta  = beta;
}
