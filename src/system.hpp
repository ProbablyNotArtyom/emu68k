//---------------------------------------------------
//
//	emu68k v0.7
//	NotArtyom
//	27/03/21
//
//---------------------------------------------------

#ifndef _SYSTEM_HEADER
#define _SYSTEM_HEADER

//---------------------------------------------------

	#include <cstdio>
	#include <cstdlib>
	#include <cstdbool>
	#include <cctype>
	#include <cstring>
	#include <gtk/gtk.h>
	#include <vte/vte.h>

	#include "musashi/m68k.h"
	#include "emu68k.hpp"

//---------------------------------------------------

#define UI_UPDATE_REG(a, b) 								\
	tmp = GTK_ENTRY(gtk_builder_get_object(builder, a));	\
	sprintf(tmpstr, "%08X", m68k_get_reg(NULL, b));			\
	gtk_entry_set_text(tmp, tmpstr);

//---------------------------------------------------

extern int	vte_char_delay;
extern bool	doExit, freerun;
extern int	steps;

extern int			cpu_type, cpu_trap_charin, cpu_trap_charout;
extern unsigned int	cpu_fcodes;

extern u_int8_t		*addrspace_ram;
extern u_int32_t	ram_start, ram_end;
extern u_int8_t		*addrspace_rom;
extern u_int32_t	rom_start, rom_end;
extern u_int32_t	uart_data, uart_status;

//---------------------------------------------------

void init_ui();
void update_sysconfig();
void update_ui_regs();
void load_rom(char *fname);
int cpu_irq_ack(int level);
void make_hex(char* buff, unsigned int pc, unsigned int length);

//---------------------------------------------------

#endif
