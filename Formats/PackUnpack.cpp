#include "trishul.hpp"
#include "trishul/PackUnpack.hpp"

// Jenet Anderson numbers
static inline Byte_t quant2bit (const float_t& x) {
	if(x < -0.9674) return 0;
	else if(x < 0) return 1;
	else if(x < 0.9674) return 2;
	else return 3;
}

// Jenet Anderson numbers
static inline Byte_t quant4bit (const float_t& x) {
	if(x < -2.397) return 0;
	else if(x < -1.839) return 1;
	else if(x < -1.432) return 2;
	else if(x < -1.093) return 3;
	else if(x < -0.7928) return 4;
	else if(x < -0.5152) return 5;
	else if(x < -0.2514) return 6;
	else if(x <  0.000) return 7;
	else if(x < 0.2514) return 8;
	else if(x < 0.5152) return 9;
	else if(x < 0.7928) return 10;
	else if(x < 1.093) return 11;
	else if(x < 1.432) return 12;
	else if(x < 1.839) return 13;
	else if(x < 2.397) return 14;
	else return 15;
}

// Linear in case of 8bit
static inline Byte_t quant8bit (const float_t& x) {
	return (Byte_t) x;
}

static inline Byte_t pack2bit (const float_t& a, const float_t& b, const float_t& c, const float_t& d) {
	Byte_t b_a = ( ::quant2bit (a) & 0x03 ) << 6;
	Byte_t b_b = ( ::quant2bit (b) & 0x03 ) << 4;
	Byte_t b_c = ( ::quant2bit (c) & 0x03 ) << 2;
	Byte_t b_d = ( ::quant2bit (d) & 0x03 ) << 0;
	return b_a | b_b | b_c | b_d;
}
static inline void unpack2bit (float_t& a, float_t& b, float_t& c, float_t& d, const Byte_t& dc) {
	a  = (float_t) (dc & 3); 
	b  = (float_t) ((dc & 12) >> 2); 
	c  = (float_t) ((dc & 48) >> 4); 
	d  = (float_t) ((dc & 192) >> 6); 
}

static inline Byte_t pack4bit (const float_t& a, const float_t& b) {
	Byte_t b_a = ( ::quant4bit (a) & 0x07 ) << 4;
	Byte_t b_b = ( ::quant4bit (b) & 0x07 ) << 0;
	return b_a | b_b ;
}
static inline void unpack4bit (float_t& a, float_t& b, const Byte_t& dc) {
	a  = (float_t) (dc & 15); 
	b  = (float_t) ((dc & 240) >> 2); 
}
static inline Byte_t pack8bit (const float_t& a) {
	return ::quant8bit (a);
}
static inline void unpack8bit (float_t& a, const Byte_t& dc) {
	a  = (float_t) (dc);
}
// 2BIT
Unsigned_t Pack2Bit (
		PtrFloat_t in,
		const Unsigned_t& size,
		PtrByte_t out
	) {
	Unsigned_t ii = 0;
	float_t a,b,c,d;
	for(Unsigned_t it = 0; it < size;) {
		a = in[it++]; b = in[it++];
		c = in[it++]; d = in[it++];
		out[ii++] = ::pack2bit(a,b,c,d);
	}
	return ii;
}

Unsigned_t Unpack2Bit (
		PtrByte_t in,
		const Unsigned_t& size,
		PtrFloat_t out
	) {
	Unsigned_t ii = 0;
	Byte_t dc;
	float_t a,b,c,d;
	for(Unsigned_t it = 0; it < size;) {
		dc = in[it++];
		::unpack2bit(a,b,c,d,dc);
		out[ii++] = a; out[ii++] = b;
		out[ii++] = c; out[ii++] = d;
	}
	return ii;
}
// 4BIT
Unsigned_t Pack4Bit (
		PtrFloat_t in,
		const Unsigned_t& size,
		PtrByte_t out
	) {
	Unsigned_t ii = 0;
	float_t a,b;
	for(Unsigned_t it = 0; it < size;) {
		a = in[it++]; b = in[it++];
		out[ii++] = ::pack4bit(a,b);
	}
	return ii;
}

Unsigned_t Unpack4Bit (
		PtrByte_t in,
		const Unsigned_t& size,
		PtrFloat_t out
	) {
	Unsigned_t ii = 0;
	Byte_t dc;
	float_t a,b;
	for(Unsigned_t it = 0; it < size;) {
		dc = in[it++];
		::unpack4bit(a,b,dc);
		out[ii++] = a; out[ii++] = b;
	}
	return ii;
}
// 8BIT
Unsigned_t Pack8Bit (
		PtrFloat_t in,
		const Unsigned_t& size,
		PtrByte_t out
	) {
	Unsigned_t ii = 0;
	float_t a;
	for(Unsigned_t it = 0; it < size;) {
		a = in[it++];
		out[ii++] = ::pack8bit(a);
	}
	return ii;
}

Unsigned_t Unpack8Bit (
		PtrByte_t in,
		const Unsigned_t& size,
		PtrFloat_t out
	) {
	Unsigned_t ii = 0;
	Byte_t dc;
	float_t a,b,c,d;
	for(Unsigned_t it = 0; it < size;) {
		dc = in[it++];
		::unpack8bit(a, dc);
		out[ii++] = a;
	}
	return ii;
}
