#include "trishul.hpp"
#include "trishul/Filterbank.hpp"
// initializing that statics
char     Filterbank::__r[64]  = "\0";
FILE*    Filterbank::fp   = nullptr;
string_t Filterbank::__hs = {"HEADER_START"};
string_t Filterbank::__he = {"HEADER_END"};
string_t Filterbank::__sr = {"src_raj"};
string_t Filterbank::__sd = {"src_dej"};
string_t Filterbank::__tt = {"tstart"};
string_t Filterbank::__tp = {"tsamp"};
string_t Filterbank::__f1 = {"fch1"};
string_t Filterbank::__df = {"foff"};
string_t Filterbank::__nc = {"nchans"};
string_t Filterbank::__ti = {"telescope_id"};
string_t Filterbank::__dt = {"data_type"};
string_t Filterbank::__nb = {"nbits"};
string_t Filterbank::__bc = {"barycentric"};
string_t Filterbank::__nf = {"nifs"};
string_t Filterbank::__sn = {"source_name"};

bool Filterbank::ReadHeader (Header_t& h, Trigger_t& t) {
  // first read
  string_t s = recv();

  // if not start rewind
  if (s != __hs || ftell (fp) != 0L) {
    rewind (fp);
    s = recv ();
  }

  // begin
  unsigned_t readh = 0;
  int idummy = 0;
  for (;s != __he; s = recv(), readh++) {
    if (s == __tp) recv (h.tsamp); 
    else if (s == __tt) recv (h.tstart);
    else if (s == __f1) recv (h.fch1);
    else if (s == __df) recv (h.foff);
    else if (s == __ti) recv (h.stationid);
    else if (s == __nc) recv (h.nchans);
    else if (s == __nb) recv (h.nbits);
    else if (s == __sr) recv (h.ra);
    else if (s == __sd) recv (h.dec);
    else if (s == __sn)  strcpy(h.name, recv ().c_str());
    // that things have to be sent
    else if (s == __bc) recv (idummy);
    else if (s == __dt) recv (idummy);
    else if (s == __nf) recv (idummy);
  }
  headersize = ftell (fp);
  fseek (fp, 0L, SEEK_END);
  nsamps = ftell (fp) - headersize;
  fseek (fp, headersize, SEEK_SET);
  return true;
}

bool Filterbank::WriteHeader (const Header_t& h, const Trigger_t& t) {
  // we don't need to send trigger in
  // dance of functions
  send(__hs);
  // source_name
  send(__sn); 
  send(h.name);
  // barycentric
  send(__bc, 0);
  // telescope ID
  send(__ti, h.stationid);
  // positions
  send(__sr, sigproc_ra(h.ra));
  send(__sd, sigproc_dec(h.dec));
  // datatype
  send(__dt, 1);
  // freq
  send(__f1, h.fch1);
  send(__df, h.foff);
  // data
  send(__nc, h.nchans);
  send(__nb, h.nbits);
  send(__nf, 1);
  // time
  send(__tt, h.tstart);
  // division by 1e6 bc DADA tsamp unit is us
  send(__tp, h.tsamp/1e6);
  send(__he);

  return true;
}

Unsigned_t Filterbank::WriteData (ByteVector_t& b, Unsigned_t n) {
  return fwrite (b.data(), 1, n ? n : b.size(), fp);
}

Unsigned_t Filterbank::ReadData (ByteVector_t& b, Unsigned_t n) {
  // headersize check
  if (ftell (fp) != headersize) {
    fseek (fp, headersize, SEEK_SET);
  }

  // resize trick
  b.resize (n, 0);

  // read
  return fread (b.data(), 1, n, fp);
}

