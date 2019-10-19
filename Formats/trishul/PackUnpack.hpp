#pragma once
#include "trishul.hpp"

void Unpack2Bit_Alloc (PtrFloat_t, const Unsigned_t& bsize);
void Pack2Bit_Alloc (PtrByte_t, const Unsigned_t& fsize);
Unsigned_t Unpack2Bit (PtrByte_t , const Unsigned_t& size, PtrFloat_t);
Unsigned_t Pack2Bit (PtrFloat_t , const Unsigned_t& size, PtrByte_t);

void Unpack4Bit_Alloc (PtrFloat_t, const Unsigned_t& bsize);
void Pack4Bit_Alloc (PtrByte_t, const Unsigned_t& fsize);
Unsigned_t Unpack4Bit (PtrByte_t , const Unsigned_t& size, PtrFloat_t);
Unsigned_t Pack4Bit (PtrFloat_t , const Unsigned_t& size, PtrByte_t);

void Unpack8Bit_Alloc (PtrFloat_t, const Unsigned_t& bsize);
void Pack8Bit_Alloc (PtrByte_t, const Unsigned_t& fsize);
Unsigned_t Unpack8Bit (PtrByte_t , const Unsigned_t& size, PtrFloat_t);
Unsigned_t Pack8Bit (PtrFloat_t , const Unsigned_t& size, PtrByte_t);
