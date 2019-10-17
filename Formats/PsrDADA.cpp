#include "trishul/PsrDADA.hpp"

PsrDADA::PsrDADA (
		key_t     _dada_key;
		) : 
	dada_key (_dada_key) {
		// initialize
		hdu = nullptr;
		// state variables
		dada_error = false;
		read_lock  = false;
		write_lock = false;
}

inline bool PsrDADA::Connect () {
	dada_error = dada_hdu_connect (hdu) < 0;
	if (dada_error) {
		return false;
	}

	// learn meta data
	header_nbufs  = ipcbuf_get_nbufs ( (ipcbuf_t*)hdu->header_block );
	header_buffsz = ipcbuf_get_bufsz ( (ipcbuf_t*)hdu->header_block );
	data_nbufs  = ipcbuf_get_nbufs ( (ipcbuf_t*)hdu->data_block );
	data_buffsz = ipcbuf_get_bufsz ( (ipcbuf_t*)hdu->data_block );
}

inline bool PsrDADA::Disconnect () {
	if (hdu) return dada_hdu_disconnect (hdu);
	else return true;
}

bool PsrDADA::ReadLock (bool x) {
	bool ret = true;
	if(x) {
		// Requested lock
		if(read_lock) ret =  true;
		else ret = dada_hdu_lock_read(hdu);
		read_lock = true;
	}
	else {
		// Requested unlock
		if(read_lock) ret = dada_hdu_unlock_read(hdu);
		else ret = true;
		read_lock = false;
	}
	return ret;
}

bool PsrDADA::WriteLock (bool x) {
	bool ret;
	if(x) {
		// Requested lock
		if(write_lock) ret = true;
		else ret = dada_hdu_lock_write(hdu);
		write_lock = true;
	}
	else {
		// Requested unlock
		if(write_lock) ret = dada_hdu_unlock_write(hdu);
		else ret = true;
		write_lock = false;
	}
	return ret;
}

bool PsrDADA::ReadHeader (Header_t& h) {
	h = {};
	// blocking read
	header = ipcbuf_get_next_read(hdu->header_block, &header_buffsz);
	if(!header) {
		multilog(log,LOG_INFO,"PsrDADA::ReadHeader key=%x GetNextRead failed\n",dada_key);
		dada_error = true;
		return false;
	}
	// get params
	if(ascii_header_get(header, "STATIONID", "%d", &( h.stationid )) < 0) {
		std::cerr << "PsrDADA::ReadHeader STATIONID write fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_get(header, "NCHAN", "%d", &( h.nchans )) < 0) {
		std::cerr << "PsrDADA::ReadHeader NCHAN read fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_get(header, "BANDWIDTH", "%lf", &( h.bandwidth )) < 0) {
		if(ascii_header_get(header, "BW", "%lf", &bandwidth) < 0) {
			std::cerr << "PsrDADA::ReadHeader BANDWIDTH read fail" << std::endl;
			dada_error = true;
		}
	}
	if(ascii_header_get(header, "CFREQ", "%lf", &( h.cfreq )) < 0) {
		if(ascii_header_get(header, "FREQ", "%f", &cfreq) < 0) {
			std::cerr << "PsrDADA::ReadHeader FREQUENCY read fail" << std::endl;
			dada_error = true;
		}
	}
	if (!dada_error) {
		double start_freq = cfreq - (bandwidth / 2);
		double chan_width = bandwidth / nchans;
		double first_chan_cfreq = start_freq + (chan_width / 2);
		fch1 = first_chan_cfreq;
		foff = chan_width;
	}
	if(ascii_header_get(header, "NPOL", "%d", &( h.npol )) < 0) {
		std::cerr << "PsrDADA::ReadHeader NPOL read fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_get(header, "NBIT", "%d", &( h.nbits )) < 0) {
		std::cerr << "PsrDADA::ReadHeader NBIT read fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_get(header, "TSAMP", "%lf", &( h.tsamp )) < 0) {
		std::cerr << "PsrDADA::ReadHeader TSAMP read fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_get(header, "SCANSTART", "%lf", &( h.tstart )) < 0) {
		std::cerr << "PsrDADA::ReadHeader SCANSTART read fail" << std::endl;
		dada_error = true;
	}
	// unix epoch
	if(ascii_header_get(header, "UNIXEPOCH", "%lf", &( h.epoch )) < 0) {
		std::cerr << "PsrDADA::ReadHeader UNIXEPOCH read fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_get(header, "RA", "%lf", &( h.ra )) < 0) {
		std::cerr << "PsrDADA::ReadHeader RA read fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_get(header, "DEC", "%lf", &( h.dec )) < 0) {
		std::cerr << "PsrDADA::ReadHeader DEC read fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_get(header, "NAME", "%s", h.name) < 0) {
		std::cerr << "PsrDADA::ReadHeader NAME read fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_get(header, "UTC_START", "%s", h.utc_start_str) < 0) {
		std::cerr << "PsrDADA::ReadHeader UTC_START read fail" << std::endl;
		dada_error = true;
	}
	// mark buffer clear
	ipcbuf_mark_cleared(hdu->header_block);	
	multilog(log,LOG_INFO,"PsrDADA::ReadHeader key=%x h_readtimes=%" PRIu64 "\n",dada_key,h_readtimes++);
	return true;
}

bool PsrDADA::WriteHeader (const Header_t& h) {
	header = ipcbuf_get_next_write(hdu->header_block);
	if(!header) {
		multilog(log,LOG_INFO,"PsrDADA::WriteHeader key=%x GetNextWrite failed\n",dada_key);
		dada_error = true;
		return false;
	}
	// set params
	if(ascii_header_set(header, "STATIONID", "%d", h.stationid) < 0) {
		std::cerr << "PsrDADA::WriteHeader STATIONID write fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_set(header, "NCHAN", "%d", h.nchans) < 0) {
		std::cerr << "PsrDADA::WriteHeader NCHAN write fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_set(header, "BANDWIDTH", "%lf", h.bandwidth) < 0) {
		if(ascii_header_set(header, "BW", "%lf", h.bandwidth) < 0) {
			std::cerr << "PsrDADA::WriteHeader BANDWIDTH write fail" << std::endl;
			dada_error = true;
		}
	}
	if(ascii_header_set(header, "CFREQ", "%lf", h.cfreq) < 0) {
		if(ascii_header_set(header, "FREQ", "%f", cfreq) < 0) {
			std::cerr << "PsrDADA::WriteHeader FREQUENCY write fail" << std::endl;
			dada_error = true;
		}
	}
	if(ascii_header_set(header, "NPOL", "%d", h.npol) < 0) {
		std::cerr << "PsrDADA::WriteHeader NPOL write fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_set(header, "NBIT", "%d", h.nbits) < 0) {
		std::cerr << "PsrDADA::WriteHeader NBIT write fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_set(header, "TSAMP", "%lf", h.tsamp) < 0) {
		std::cerr << "PsrDADA::WriteHeader TSAMP write fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_set(header, "SCANSTART", "%lf", h.tstart) < 0) {
		std::cerr << "PsrDADA::WriteHeader SCANSTART write fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_set(header, "UNIXEPOCH", "%lf", h.epoch) < 0) {
		std::cerr << "PsrDADA::WriteHeader UNIXEPOCH write fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_set(header, "RA", "%lf", h.ra) < 0) {
		std::cerr << "PsrDADA::WriteHeader RA write fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_set(header, "DEC", "%lf", h.dec) < 0) {
		std::cerr << "PsrDADA::WriteHeader DEC write fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_set(header, "NAME", "%s", h.name) < 0) {
		std::cerr << "PsrDADA::WriteHeader NAME write fail" << std::endl;
		dada_error = true;
	}
	if(ascii_header_set(header, "UTC_START", "%s", h.utc_start_str) < 0) {
		std::cerr << "PsrDADA::WriteHeader UTC_START write fail" << std::endl;
		dada_error = true;
	}
	ipcbuf_mark_filled (hdu->header_block, header_size);
	multilog(log,LOG_INFO,"PsrDADA::WriteHeader key=%x h_writetimes=%" PRIu64 "\n",dada_key,h_writetimes++);
	return true;
}

Unsigned_t PsrDADA::ReadData (PtrByte_t packed_data, Unsigned_t to_read = 0L) {
	if (to_read == 0L) to_read = data_buffsz;

	Unsigned_t read = ipcio_read (
			hdu->data_block, reinterpret_cast<char*>(packed_data),
			to_read	
	);
	return to_read - read;
}

Unsigned_t PsrDADA::WriteData (PtrByte_t packed_data, Unsigned_t to_write = 0L) {
	if (to_read == 0L) to_write = data_buffsz;

	Unsigned_t wrote = ipcio_write (
			hdu->data_block, reinterpret_cast<char*>(packed_data),
			to_read	
	);

	return to_write - wrote;
}
