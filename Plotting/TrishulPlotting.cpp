#include "trishul.hpp"
#include "trishul/TrishulPlotting.hpp"

void TrishulPlotting::Fscrunch (const FloatVector_t& fin, Unsigned_t& nsamps, FloatVector_t& fout) {
	std::cout << fin.size() << std::endl;
	std::cout << fout.size() << std::endl;
	std::cout << nsamps << std::endl;
	std::cout << chanin << std::endl;
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

void TrishulPlotting::ABShape (const FloatVector_t& f, const Unsigned_t& nsamps, const unsigned_t& nchans, FloatVector_t& tshape, FloatVector_t& bshape) {
	Unsigned_t idx = 0;
	float_t    idx_element = 0.0f;

	std::fill (tshape.begin(), tshape.end(), 0.0f);
	std::fill (bshape.begin(), bshape.end(), 0.0f);

	for (Unsigned_t isamp = 0; isamp < nsamps; isamp++) {
		for (unsigned_t ichan = 0; ichan < nchans; ichan++) {
			idx = isamp * nchans + ichan;
			idx_element = f[idx];

			bshape[ichan] += (idx_element/nsamps);
			tshape[isamp] += (idx_element/nchans);
		}
	}

}

