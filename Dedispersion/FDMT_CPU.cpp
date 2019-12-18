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
	freq_off = _freq_off;
	// 
	fmax     = _freq_ch1;
	fmin     = fmax + (nchans*freq_off);
}

void fc::Execute (
  const FloatVector_t& in,
  Unsigned_t _nsamps,
  FloatVector_t& out) {
  nsamps = _nsamps;

  // FDMT start
  Initialization (in);
  for (unsigned_t i = 1; i < l2nch; i++)
  	Iteration (i);
  // FDMT end
  
  // slicing State(T,1,aDT:bDT) = State(T,aDT:bDT)
  auto wdt = bDT - aDT;
  out.resize (wdt * nsamps);
	for (Unsigned_t isamp = 0; isamp <nsamps; isamp++)
	for (Unsigned_t idt = 0; idt <wdt; idt++)
	out [idt + wdt*isamp] = State [idt +aDT +maxDT*isamp];
}

void fc::Iteration (const unsigned_t& i) {
	Unsigned_t deltaF = std::pow (2, i) * freq_off;
	Unsigned_t deltaT = std::ceil ( (maxDT-1)*cff(fmin, fmin+deltaF, fmax) );

	unsigned_t fjumps = std::pow (2,i-1);
	
	FloatVector_t Output (fjumps*nsamps*(deltaT+1));
	Unsigned_t alpha = deltaT+1;
	Unsigned_t beta  = alpha*fjumps;

	for (unsigned_t iF = 0; iF < fjumps; iF++) {
		float_t l_fres = (fmax - fmin) / fjumps;

		float_t fstart = (l_fres * iF) + fmin;
		float_t fend   = (l_fres * (iF+1)) + fmin;
		float_t fmid   = 0.5*(fend + fstart);
		float_t fmidl  = 0.5*(fend + fstart) + (i ? 0.5*freq_off : 0);

		Unsigned_t l_dT = std::ceil ((maxDT-1)*dff(fstart,fend)/dff(fmin,fmax));
		for (Unsigned_t idt = 0; idt <= l_dT; idt++) {
			Unsigned_t midt = idt * dff (fmid, fstart)  / dff (fend, fstart);
			Unsigned_t midtl= idt * dff (fmidl, fstart) / dff (fend, fstart);
			// 
			for (Unsigned_t itt = 0; itt <midtl; itt++)
			//Output.at (iF, idt, itt) = State.at (2*iF, midt, itt);
			Output [itt + alpha*iF + beta*idt] =
			State  [itt + 2*alpha*iF + beta*midt];
			// 
			for (Unsigned_t itt = midtl; itt <nsamps; itt++)
			//Output.at (iF,idt, itt) = State.at (2*iF   , midt, itt) + State.at (2*iF +1, idt-midtl, itt-midtl);
			Output [itt + alpha*iF   + beta*idt]   = 
			State  [itt + 2*alpha*iF + beta*midt]  +
			State  [itt-midtl +2*alpha*iF +alpha +beta*idt -beta*midtl];
		}
	}

	State = std::move (Output);
}

void fc::Initialization (const FloatVector_t& Image) {
	aDT   =  4148.741601 * dff (fmax, fmin) * dm_list.front () / tsamp;
	bDT   =  4148.741601 * dff (fmax, fmin) * dm_list.back  () / tsamp;
	maxDT = bDT;
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

	State = std::move (Output);
}
