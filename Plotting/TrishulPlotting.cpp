#include "trishul.hpp"
#include "trishul/TrishulPlotting.hpp"

float_t TrishulPlotting::xxfac = 0.1;
unsigned_t TrishulPlotting::chanout = 512;
unsigned_t TrishulPlotting::chanin  = 4096;

void TrishulPlotting::Fscrunch (const FloatVector_t& fin, const Unsigned_t& nsamps, const Unsigned_t& nchans, FloatVector_t& fout) {
  unsigned_t chanout = nchans; 
	unsigned_t df  = chanin / chanout;
	float_t    rdf = 1.0f / df;

	// reset fout
	// Let me remind myself about how much time
	// I wasted in this glaring accumulation bug
	std::fill (fout.begin(), fout.end(), 0.0f);

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

void TrishulPlotting::BShape (const FloatVector_t& f, const Unsigned_t& nsamps, const unsigned_t& nchans, FloatVector_t& bshape) {
	Unsigned_t idx = 0;
	float_t    idx_element = 0.0f;

	std::fill (bshape.begin(), bshape.end(), 0.0f);

	for (Unsigned_t isamp = 0; isamp < nsamps; isamp++) {
		for (unsigned_t ichan = 0; ichan < nchans; ichan++) {
			idx = isamp * nchans + ichan;
			idx_element = f[idx];

			bshape[ichan] += (idx_element/nsamps);
		}
	}
}
void TrishulPlotting::BMaxShape (const FloatVector_t& f, const Unsigned_t& nsamps, const unsigned_t& nchans, FloatVector_t& bshape) {
	auto i = f.begin();
	auto j = f.end();

	std::fill (bshape.begin(), bshape.end(), 0.0f);

	for (unsigned_t ichan = 0; ichan < nchans; ichan++) {
		i = f.begin() + (ichan * nsamps);
		j = i + nsamps;
		bshape[ichan] = *std::max_element (i, j);
	}
}
void TrishulPlotting::ABMaxShape (const FloatVector_t& f, const Unsigned_t& nsamps, const unsigned_t& nchans,  FloatVector_t& tshape, FloatVector_t& bshape) {
	// temp
	float_t curr = 0.0f;
	float_t tmin = std::numeric_limits<float_t>::min ();
	float_t fmin = std::numeric_limits<float_t>::min ();

	// fb traversal
	for (Unsigned_t isamp = 0; isamp < nsamps; isamp++) {
		for (unsigned_t ichan = 0; ichan < nchans; ichan++) {
			// read
			curr = f[isamp*nchans + ichan];

			// logic
			if (curr > tmin) {
				tmin = curr;
				tshape[isamp] = curr;
			}
			if (curr > fmin) {
				fmin = curr;
				bshape[ichan] = curr;
			}
		}
	}
}
void TrishulPlotting::__ranger (FloatVector_t::const_iterator i, FloatVector_t::const_iterator j, 
		float_t& xxmin,
		float_t& xxmax) {
	auto min = *std::min_element (i, j);
	auto max = *std::max_element (i, j);

	auto dd_range = max - min;

	xxmin = min  - (xxfac * dd_range);
	xxmax = max  + (xxfac * dd_range);
}

void TrishulPlotting::__ranger (FloatVector_t::const_iterator i, Unsigned_t nj, 
		float_t& xxmin,
		float_t& xxmax) {
	FloatVector_t::const_iterator j = i + nj;
	auto min = *std::min_element (i, j);
	auto max = *std::max_element (i, j);

	auto dd_range = max - min;

	xxmin = min  - (xxfac * dd_range);
	xxmax = max  + (xxfac * dd_range);
}
void TrishulPlotting::__arange (FloatVector_t& ptr, float_t start, float_t step, Unsigned_t size) {
	ptr.push_back (start);
	for (Unsigned_t i = 1; i < size; i++) {
		ptr.push_back (ptr.back() + step);
	}
}

void TrishulPlotting::__zfill (FloatVector_t& ptr, Unsigned_t size) {
  ptr.resize (size, 0.0f);
}

