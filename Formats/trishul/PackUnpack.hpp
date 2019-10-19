#pragma once
#include "trishul.hpp"

void Unpack2Bit (const ByteVector_t& , FloatVector_t&);
void Pack2Bit  (const FloatVector_t&, ByteVector_t&);

void Unpack4Bit (const ByteVector_t& , FloatVector_t&);
void Pack4Bit  (const FloatVector_t&, ByteVector_t&);

void Unpack8Bit (const ByteVector_t& , FloatVector_t&);
void Pack8Bit  (const FloatVector_t&, ByteVector_t&);
