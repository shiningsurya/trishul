#pragma once
#include "trishul.hpp"
#include "trishul/Formats.hpp"

#include "nlohmann/json.hpp"

class BSON : public TrishulFormats {
	using json = nlohmann::json;
	private:
		path_t       odir;
		string_t     filename;
		char         cfile[256];
		json         j;
	public:
		Unsigned_t   nsamps;

		BSON (const string_t& _dir) : odir(_dir) {}
		BSON () : odir("./") {}
		~BSON () {}

		bool ReadFromFile (const string_t&);
		bool WriteToFile ();
		bool WriteToFile (const string_t&);

		// override
		bool ReadHeader (Header_t& , Trigger_t& ) override;
		bool WriteHeader (const Header_t&, const Trigger_t&) override;
		Unsigned_t ReadData  (ByteVector_t&, Unsigned_t, Unsigned_t = 0)override;
		Unsigned_t WriteData (ByteVector_t&, Unsigned_t = 0) override;
};

