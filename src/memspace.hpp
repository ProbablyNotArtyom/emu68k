/*####################################################################*/
//
//	emu68k v0.7
//	NotArtyom
//	27/03/21
//
/*####################################################################*/

#ifndef _MEMSPACE_HEADER
#define _MEMSPACE_HEADER

	#include <cstdio>
	#include <cstdlib>
	#include <cstdbool>
	#include <cctype>
	#include <cstring>

/*####################################################################*/

namespace MemSpace {
	extern "C" u_int cpu_read_byte(u_int addr);
	extern "C" u_int cpu_read_word(u_int addr);
	extern "C" u_int cpu_read_long(u_int addr);
	extern "C" u_int cpu_read_word_dasm(u_int addr);
	extern "C" u_int cpu_read_long_dasm(u_int addr);
	extern "C" void cpu_write_byte(u_int addr, u_int val);
	extern "C" void cpu_write_word(u_int addr, u_int val);
	extern "C" void cpu_write_long(u_int addr, u_int val);

	int clear_addrspace_ram();
	int clear_addrspace_rom();
};

/*####################################################################*/

#endif
