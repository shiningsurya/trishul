#include "trishul.hpp"
#include "trishul/Header.hpp"
#include "trishul/Filterbank.hpp"
#include "trishul/PackUnpack.hpp"
#include "trishul/FilterbankBowtiePlot.hpp"

//void fbp::__arange (FloatVector_t&, float_t, float_t, Unsigned_t);

int main() {
	string_t file("/home/shining/work/vlite/20190711_220143_muos_ea99.fil");
	// bson
	Filterbank f(file);
	const TrishulFormats& tf_f = f;
	// headers
	Header_t hh; Trigger_t tt;
	f.ReadHeader (hh, tt);
	std::cout << "Header: " << std::endl;
	std::cout << hh;
	Unsigned_t nsamps = f.nsamps / hh.nchans * 8 / hh.nbits;
	std::cout << "\tnsamps=" << nsamps << std::endl;
	// read data
	nsamps = 10.0f/hh.tsamp;
	ByteVector_t bdata;
	Unsigned_t size = f.ReadData (bdata, nsamps*hh.nchans);
	std::cout << "\tbsize=" << bdata.size() << std::endl;
	// Unpack
	FloatVector_t data;
	Unpack2Bit (bdata, data);
	std::cout << "\tsize=" << data.size() << std::endl;
	std::cout << std::endl;
	//
	// plotting
	FloatVector_t dmvec;
	TrishulPlotting::__arange (dmvec, 50.0f, 1000.0f/128, 128);
	FilterbankBowtiePlot fbp;
	fbp.Read (hh, tt);
	fbp.SetDM (dmvec);
	fbp.ReadFB (data, nsamps);
	fbp.ReadBT (data, nsamps);
	fbp.Plot ("/xwin");
	//
	return 0;
}
