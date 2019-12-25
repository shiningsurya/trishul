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
	// freq_off is positive
	freq_off = -_freq_off;
	freq_ch1 = _freq_ch1;
	fmax     = _freq_ch1;
	fmin     = fmax - (nchans*freq_off);
}

Unsigned_t fc::MaxSampDelay () noexcept {
	bDT   =  4148.741601 * dff (fmin, fmax) * dm_list.back  () / tsamp;
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
  #if 0 
  auto ddnsamps = nsamps-idelays.back();
  out.resize (idelays.size() * ddnsamps);
	for (Unsigned_t idt = 0  ; idt<idelays.size(); idt++)
	for (Unsigned_t isamp = 0; isamp <ddnsamps   ; isamp++)
	out [isamp +nsamps*idt] = State [idelays[idt] +sbeta*isamp];
	#else
	out = State;
	#endif
}

void fc::Iteration (const unsigned_t& i) {
	auto deltaF = std::pow (2, i) * freq_off;
	Unsigned_t deltaT = std::ceil ( (maxDT-1)*cff(fmin, fmin+deltaF, fmax) );

	unsigned_t fjumps = nchans / std::pow (2,i);
	
	FloatVector_t Output (fjumps*nsamps*(deltaT+1));
//std::cout << "iter=" << i << " (" << nsamps << "," << fjumps << "," << deltaT+1 << ")" << std::endl;
	Unsigned_t alpha = deltaT+1;
	Unsigned_t beta  = alpha*fjumps;

	for (unsigned_t iF = 0; iF < fjumps; iF++) {
		float_t fstart = (deltaF* iF) + fmin;
		float_t fend   = (deltaF* (iF+1)) + fmin;
		float_t fmid   = 0.5*(fend + fstart) - 0.5*freq_off;
		float_t fmidl  = 0.5*(fend + fstart) + 0.5*freq_off;

		Unsigned_t l_dT = std::ceil ((maxDT-1)*dff(fstart,fend)/dff(fmin,fmax));
		for (Unsigned_t idt = 0; idt <= l_dT; idt++) {
//std::cout << "iF=" << iF << " idt=" << idt << std::endl;
			Unsigned_t midt = idt * dff (fstart, fmid)  / dff (fstart, fend);
			Unsigned_t midtl= idt * dff (fstart, fmidl) / dff (fstart, fend);
			// 
			for (Unsigned_t itt = 0; itt <midtl; itt++) 
			//Output.at (iF, idt, itt) = State.at (2*iF, midt, itt);
			Output [idt  +alpha*iF    + beta*itt]   =
			State  [midt +salpha*iF*2 + sbeta*itt]  ;
			// 
			for (Unsigned_t itt = midtl; itt <nsamps; itt++)
			//Output.at (iF,idt, itt) = State.at (2*iF   , midt, itt) + State.at (2*iF +1, idt-midtl, itt-midtl);
			{
			Output [idt       +alpha*iF             +beta*itt]                =
			State  [midt      +2*salpha*iF          +sbeta*itt]               +
			State  [idt-midtl +2*salpha*iF  +salpha +sbeta*itt -sbeta*midtl]  ;
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
	maxDT = std::max(bDT, static_cast<Unsigned_t>(nsamps));
	Unsigned_t deltaT = std::ceil ( (maxDT-1)*cff(fmin, fmin+freq_off, fmax) );

	FloatVector_t Output (Image.size() * (deltaT+1));
	Unsigned_t alpha = deltaT+1;
	Unsigned_t beta  = alpha*nchans;
	std::copy (Image.begin(), Image.end(), Output.begin());

	for (Unsigned_t idt = 1; idt <= deltaT; idt++) 
	for (unsigned_t isamp = idt; isamp < nsamps; isamp++)
	for (unsigned_t ichan = 0; ichan < nchans; ichan++)
		//Output.at (isamp, ichan, idt) = Output.at (isamp, ichan, idt-1) + Image.at  (nsamps-isamp+1, ichan);
		Output [idt   +alpha*ichan +beta*isamp] =
		Output [idt-1 +alpha*ichan +beta*isamp] +
		Image  [ichan +nsamps*(nsamps-isamp+1)];

	State  = std::move (Output);
	salpha = alpha;
	sbeta  = beta;
}
