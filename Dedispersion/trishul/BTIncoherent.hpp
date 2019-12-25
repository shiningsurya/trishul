#pragma once
#include "trishul.hpp"
#include "trishul/Incoherent.hpp"

class BTIncoherent : public TrishulDedisperser {
  private:
    Incoherent incoh;
	public:
	  BTIncoherent ()  = default;
	  ~BTIncoherent () = default;

		void CreatePlan (const double_t, const unsigned_t, const float_t, const float_t) override;

		void Execute (const FloatVector_t&, Unsigned_t, FloatVector_t&) override;
		void Execute (const ByteVector_t&,  Unsigned_t, ByteVector_t& ) override {
			throw TrishulError ("Not available for BTIncoherent.");
		}

		Unsigned_t MaxSampDelay () noexcept {
		  return dm_list.back();
		}
};
