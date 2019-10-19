#include "trishul.hpp"
#include "trishul/TrishulPlotting.hpp"

void TrishulPlotting::Fscrunch (PtrFloat_t fin, Unsigned_t& nsamps, PtrFloat_t fout) {
	unsigned_t df  = chanin / chanout;
	float_t    rdf = 1.0f / df;

#ifdef TS_OMP
#pragma omp parallel for simd
#endif
	for (Unsigned_t isamp = 0; isamp < nsamps; isamp++) {
		for (unsigned_t k = 0; k < df; k++) {
			for (unsigned_t j = 0; j < chanout; j++) {
				fout[ (isamp*chanout) + j ] += 
					rdf * fin[ (isamp*chanin) + j + (k*chanout) ];
			}
		}
	}
}

void TrishulPlotting::ABShape (PtrFloat_t f, const Unsigned_t& nsamps, const unsigned_t& nchans, PtrFloat_t tshape, PtrFloat_t bshape) {
	Unsigned_t idx = 0;
	float_t    idx_element = 0.0f;

	std::fill (tshape, tshape + nsamps, 0.0f);
	std::fill (bshape, bshape + nchans, 0.0f);

	for (Unsigned_t isamp = 0; isamp < nsamps; isamp++) {
		for (unsigned_t ichan = 0; ichan < nchans; ichan++) {
			idx = isamp * nchans + ichan;
			idx_element = f[idx];

			bshape[ichan] += (idx_element/nsamps);
			tshape[isamp] += (idx_element/nchans);
		}
	}

}

