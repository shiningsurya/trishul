#pragma once
#include "trishul.hpp"
#include "trishul/Formats.hpp"

#include <cmath>
#include <cstring>

class Filterbank : public  TrishulFormats {
  private:
    static FILE * fp;
    string_t filename;

    Unsigned_t headersize;

    static inline double_t sigproc_ra(double_t ra) {
      float_t hh = (180.0f / M_PI) * (24.0f / 360.0f) * ra;
      float_t mm = (hh - int(hh))  * 60.0f;
      float_t ss = (mm - int(mm))  * 60.0f;
      return ( int(hh) * 1e4 ) + ( int(mm) * 1e2 ) + ss;
    }
    static inline double_t sigproc_dec(double_t dec) {
      float_t dd = (180.0f / M_PI) * fabs(dec);
      float_t mm = (dd - int(dd))  * 60.0f;
      float_t ss = (mm - int(mm))  * 60.0f;
      return ( int(dd) * 1e4 ) + ( int(mm) * 1e2 ) + ss;
    }

    static inline void send (const string_t& str) {
      unsigned_t len = str.length();
      fwrite (&len, sizeof(int), 1,  fp);
      fwrite (str.c_str(),  sizeof(char),len,fp);
    }
    template<typename T>
    static inline void send (const string_t& str, const T data) {
      send (str);
      fwrite (&data, sizeof(T), 1, fp);
    }

    static char __r[64];
    template<typename T>
    static inline void recv (T& data) {
      fread (&data, sizeof(T), 1, fp);
    }
    static inline string_t recv () {
      int len = 0;
      recv (len);
      fread (__r, 1, len, fp);
      return string_t (__r, __r + len);
    }

    // that long list
    static string_t __hs;
    static string_t __he;
    static string_t __sr;
    static string_t __sd;
    static string_t __tt;
    static string_t __tp;
    static string_t __f1;
    static string_t __df;
    static string_t __nc;
    static string_t __ti;
    static string_t __dt;
    static string_t __nb;
    static string_t __bc;
    static string_t __nf;
    static string_t __sn;
  public:
		Unsigned_t   nsamps;
    // tors
    Filterbank (const string_t& fbname) : filename(fbname) {
      Open ( fbname );
    }
    ~Filterbank () { Close(); }

    // DiagPrint
    void DiagPrint () {
      std::cout << "  Filterbank::DiagPrint headersize=" << headersize << std::endl;
      std::cout << "  Filterbank::DiagPrint datasize=" << nsamps << std::endl;
    }

    // open/close
    void Open (const string_t& fn)  {
      filename = fn;
      fp = fopen (filename.c_str(), "rwb+");
      if (fp == NULL) {
        throw TrishulError ("Filterbank unable to open!");
      }
    }
    void Close () {
      if (fp) {
        fclose (fp);
        fp = nullptr;
      }
    }

		// override
		bool ReadHeader (Header_t& , Trigger_t& ) override;
		bool WriteHeader (const Header_t&, const Trigger_t&) override;
		Unsigned_t ReadData  (ByteVector_t&, Unsigned_t, Unsigned_t)override;
		Unsigned_t WriteData (ByteVector_t&, Unsigned_t) override;
};
