#pragma once

//didn't wanted to waste time dealing with compiler complainations and wrote these procs

//dest, src, bytes
extern "C" __cdecl void strncpy(void*, const void*, unsigned int);

//like memset but instead of settings "bytes" it sets dwords
//dest, value, count of dwords
extern "C" __cdecl void memset4(void*, unsigned int, unsigned int);