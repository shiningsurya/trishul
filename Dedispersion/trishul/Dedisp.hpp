

class Dedisp : protected TrishulDedisperser {
	private:
		// dedisp
		dedisp_plan        dplan;
		dedisp_error       error;
	public:
		Dedisp () ;
		Dedisp (
				const unsigned_t _nchans, 
				const double_t   _tsamp,
				const float_t    _freq_ch1,
				const float_t    _freq_off
		);
		~Dedisp () {
			if (_is_plan_initialized)
				dedisp_destroy_plan (dplan);
		}
		void Execute override final (

		);

		// deleted copy/move
		Dedisp  (const Dedisp& ) = delete; 
		Dedisp& (const Dedisp)   = delete;
		
};
