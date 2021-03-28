//---------------------------------------------------
//
//	emu68k v0.7
//	NotArtyom
//	27/03/21
//
//---------------------------------------------------

	#include <cstdio>
	#include <cstdlib>
	#include <cstdbool>
	#include <cctype>
	#include <cstring>

	#include "musashi/m68k.h"
	#include "memspace.hpp"
	#include "emu68k.hpp"
	#include "system.hpp"
	#include "vinput.hpp"

//---------------------------------------------------

namespace MemSpace {
	extern "C" u_int cpu_read_byte(u_int addr){
		if (addr >= ram_start && addr <= ram_end) return READ_BYTE(addrspace_ram, addr);
		else if (addr >= rom_start && addr <= rom_end) return READ_BYTE(addrspace_rom, addr);
		else if (addr == uart_data) return uart_input_buff;
		else if (addr == uart_status) {
			if (uart_status_byte == 0b00000101) {
				uart_status_byte = 0b00000100;
				return 0b00000101;
			} else return 0b00000100;
		} else return 0x00;
	}

	extern "C" u_int cpu_read_word(u_int addr){
		if (addr >= ram_start && addr <= ram_end) return READ_WORD(addrspace_ram, addr);
		else if (addr >= rom_start && addr <= rom_end) return READ_WORD(addrspace_rom, addr);
		else if (addr == uart_data) return uart_input_buff;
		else if (addr == uart_status) {
			if (uart_status_byte == 0b00000101) {
				uart_status_byte = 0b00000100;
				return 0b00000101;
			} else return 0b00000100;
		} else return 0x00;
	}

	extern "C" u_int cpu_read_long(u_int addr){
		if (addr >= ram_start && addr <= ram_end) return READ_LONG(addrspace_ram, addr);
		else if (addr >= rom_start && addr <= rom_end) return READ_LONG(addrspace_rom, addr);
		else if (addr == uart_data) return uart_input_buff;
		else if (addr == uart_status) {
			if (uart_status_byte == 0b00000101) {
				uart_status_byte = 0b00000100;
				return 0b00000101;
			} else return 0b00000100;
		} else return 0x00;
	}

	extern "C" u_int cpu_read_word_dasm(u_int addr){
		if (addr >= ram_start && addr <= ram_end) return READ_WORD(addrspace_ram, addr);
		else if (addr >= rom_start && addr <= rom_end) return READ_WORD(addrspace_rom, addr);
		else if (addr == uart_data) return uart_input_buff;
		else if (addr == uart_status) {
			if (uart_status_byte == 0b00000101) {
				uart_status_byte = 0b00000100;
				return 0b00000101;
			} else return 0b00000100;
		} else return 0x00;
	}

	extern "C" u_int cpu_read_long_dasm(u_int addr){
		if (addr >= ram_start && addr <= ram_end) return READ_LONG(addrspace_ram, addr);
		else if (addr >= rom_start && addr <= rom_end) return READ_LONG(addrspace_rom, addr);
		else if (addr == uart_data) return uart_input_buff;
		else if (addr == uart_status) {
			if (uart_status_byte == 0b00000101) {
				uart_status_byte = 0b00000100;
				return 0b00000101;
			} else return 0b00000100;
		} else return 0x00;
	}

	/* Write to anywhere */
	extern "C" void cpu_write_byte(u_int addr, u_int val){
		if (addr >= ram_start && addr <= ram_end) WRITE_BYTE(addrspace_ram, addr, val);
		else if (addr >= rom_start && addr <= rom_end) WRITE_BYTE(addrspace_rom, addr, val);
		else if (addr == uart_data) vte_putchar(&val);
		else if (addr == uart_status) return;
		else return;
	}

	extern "C" void cpu_write_word(u_int addr, u_int val){
		if (addr >= ram_start && addr <= ram_end) WRITE_WORD(addrspace_ram, addr, val);
		else if (addr >= rom_start && addr <= rom_end) WRITE_WORD(addrspace_rom, addr, val);
		else if (addr == uart_data) vte_putchar(&val);
		else if (addr == uart_status) return;
		else return;
	}

	extern "C" void cpu_write_long(u_int addr, u_int val){
		if (addr >= ram_start && addr <= ram_end) WRITE_LONG(addrspace_ram, addr, val);
		else if (addr >= rom_start && addr <= rom_end) WRITE_LONG(addrspace_rom, addr, val);
		else if (addr == uart_data) vte_putchar(&val);
		else if (addr == uart_status) return;
		else return;
	}

//---------------------------------------------------

	int clear_addrspace_ram() {
		return (memset(addrspace_ram, 0xFF, ram_end - ram_start) != NULL);
	}

	int clear_addrspace_rom() {
		return (memset(addrspace_rom, 0xFF, rom_end - rom_start) != NULL);
	}
}
