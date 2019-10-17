#pragma once
#include "trishul.hpp"
// One Header to rule them all

typedef struct {
	// station
	unsigned_t          stationid;
	// positions
	double_t            ra; 
	double_t            dec;
	// frequency
	double_t            fch1;
	double_t            foff;
	double_t            cfreq;
	double_t            bandwidth;
	// time
	double_t            tsamp;
	double_t            tstart;
	double_t            epoch;
	// memory
	unsigned_t          nbits;
	unsigned_t          nchans;
	unsigned_t          npol;
	// strings
	char                utc_start_str[16];
	char                name[16];
	char                group[16];
} Header_t;


typedef struct {
	double_t       i0;
	double_t       i1;
	float_t        sn;
	float_t        dm;
	float_t        width;
	float_t        peak_time;
	char           meta[128];
} Trigger_t;

std::ostream& operator<<(std::ostream& os, const Header_t& f);

std::ostream& operator<<(std::ostream& os, const Trigger_t& f);
