#include "trishul.hpp"
#include "trishul/BSON.hpp"


bool BSON::ReadFromFile (const string_t& filename) {
	std::ifstream ifs (filename, std::ios::binary);
	std::vector<uint8_t> vb;
	if( ifs.good() ) {
		 while( !ifs.eof() ) vb.push_back(ifs.get()); 
	}
	else {
		ifs.close();
		return false;
	}

	std::cout << "back=" << vb.back() << std::endl;
	vb.pop_back(); 
	// one for std::endl <- my fault
	std::cout << "back=" << vb.back() << std::endl;
	vb.pop_back();
	// one for EOF
	j = json::from_ubjson (vb);

	return true;
}

bool BSON::WriteToFile () {
	auto ofile = odir / filename;
	std::fstream fs (ofile.string(), std::ios::binary);
	if (!fs.good()) {
		return false;
	}
	// It is understood that
	// j is to be written
	
	std::vector<Byte_t> j_bson = json::to_ubjson (j);

	std::ostream_iterator<Byte_t> oo(fs);
	std::copy (
			j_bson.begin(), j_bson.end(),
			oo
	);

	fs.close();
	return true;
}

bool BSON::WriteToFile (const string_t& fname) {
	auto ofile = odir / fname;
	std::fstream fs (ofile.string(), std::ios::binary);
	if (!fs.good()) {
		return false;
	}
	// It is understood that
	// j is to be written
	
	std::vector<Byte_t> j_bson = json::to_ubjson (j);

	std::ostream_iterator<Byte_t> oo(fs);
	std::copy (
			j_bson.begin(), j_bson.end(),
			oo
	);

	fs.close();
	return true;
}

bool BSON::ReadHeader (Header_t& h, Trigger_t& t) {
	// Trigger_t
	t.sn        = j["sn"];
	t.dm        = j["dm"];
	t.width     = j["width"];
	// header time
	h.tsamp     = j["time"]["tsamp"];
	// tsamp is always << 1
	if (h.tsamp >= 1) h.tsamp /= 1E6;
	// header frequency
	h.fch1      = j["frequency"]["fch1"];
	h.foff      = j["frequency"]["foff"];
	h.nchans    = j["frequency"]["nchans"];

	// header parameters
	//h.nbits     = j["parameters"]["nbits"];
	//h.stationid = j["parameters"]["antenna"];
	//h.ra        = j["parameters"]["ra"];
	//h.dec       = j["parameters"]["dec"];
	//This roundabout way because ra/dec was 
	//introduced in a later version
	auto jp = j["parameters"];
	for (const auto& ijp : jp.items()) {
	  if (ijp.key() == "nbits") h.nbits = ijp.value();
	  else if (ijp.key() == "antenna") h.stationid = ijp.value();
	  else if (ijp.key() == "ra") h.ra = ijp.value();
	  else if (ijp.key() == "dec") h.dec = ijp.value();
	}
	// some book-keeping 
	//duration = j["time"]["duration"];
	t.peak_time = j["time"]["peak_time"];
	h.tstart    = j["time"]["tstart"];
	// string_t to char*
	string_t x  = j["parameters"]["source_name"];
	strcpy (h.name, x.c_str());
	string_t y  = j["parameters"]["group"];
	strcpy (h.group, y.c_str());

	nsamps = j["indices"]["nsamps"];

	h_readtimes++;
	return true;
}

bool BSON::WriteHeader (const Header_t& head, const Trigger_t& trig)  {
	// header
	j["sn"] = trig.sn;
	j["dm"] = trig.dm;
	j["width"] = trig.width;
	// header time
	j["time"]["tsamp"] = head.tsamp;
	// header frequency
	j["frequency"]["fch1"] = head.fch1;
	j["frequency"]["foff"] = head.foff;
	j["frequency"]["nchans"] = head.nchans;
	// header indices
	float_t dur = std::ceil(trig.i1 - trig.i0);
	nsamps = trig.i1 >= trig.i0 ? dur/head.tsamp/1e-6 * head.nchans * head.nbits / 8 : 0L;
	j["indices"]["nsamps"] = nsamps;
	// header parameters
	j["parameters"]["nbits"] = head.nbits;
	j["parameters"]["antenna"] = head.stationid;
	j["parameters"]["source_name"] = head.name;
	j["parameters"]["ra"] = head.ra;
	j["parameters"]["dec"] = head.dec;
	// some book-keeping 
	double_t sec_from_start = trig.i0 - head.epoch;
	j["time"]["tstart"] = head.tstart + (sec_from_start/86400.0f);
	j["time"]["peak_time"] = trig.peak_time - sec_from_start;

#if 0
	// group
	struct tm utc_time;
	time_t hepoch = head.epoch;
	gmtime_r (&hepoch, &utc_time);
	strftime (head.group, sizeof(head.group), "%Y%m%d_%H%M%S", &utc_time);
#endif 
	j["parameters"]["group"] = std::string(head.group);
	snprintf(cfile, sizeof(cfile),
			"%s_muos_ea%02d_dm%3.2f_sn%2.2f.fbson", head.group, head.stationid,trig.dm,trig.sn 
			);

	filename = std::string(cfile);
	h_writetimes++;
	return true;
}

Unsigned_t BSON::ReadData  (ByteVector_t& data, Unsigned_t bytes, Unsigned_t offset) {
	bool ret = false;
	auto v_fb = j["fb"];
	if (bytes == 0) {
		// read entire file
		bytes = v_fb.size();
		ret = true;
	}
	data.resize (bytes + offset, 0);

	std::copy (v_fb.begin(), v_fb.begin() + bytes, data.begin());

	d_readtimes++;
	return ret ? v_fb.size() : v_fb.size() - bytes;
}

Unsigned_t BSON::WriteData (ByteVector_t& data, Unsigned_t bytes) {
	if (bytes == 0) bytes = data.size();
	std::vector<Byte_t> v_fb;

	std::copy (data.cbegin(), data.cbegin() + bytes, std::back_inserter(v_fb));

	j["fb"] = v_fb;
	d_writetimes++;
	return true;
}

