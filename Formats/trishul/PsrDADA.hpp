#include "trishul.hpp"
#include "trishul/Formats.hpp"

class PsrDADA : protected TrishulFormats {
	private:
		// state
		bool              dada_error;
		bool              read_lock;
		bool					    write_lock;
		// dada
		key_t             dada_key;
		dada_hdu_t*       hdu;
		// sizes
		Unsigned_t        header_buffsz;
		Unsigned_t        data_buffsz;
		unsigned_t        header_nbufs;
		unsigned_t        data_nbufs;
	public:
		inline bool Connect ();
		inline bool Disconnect ();

		bool ReadLock (bool);
		bool WriteLock (bool);


		PsrDADA (key_t);
		~PsrDADA () {
			Disconnect();
		}
};

