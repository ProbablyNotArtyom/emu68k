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
	#include <gtk/gtk.h>
	#include <vte/vte.h>

	#include "musashi/m68k.h"
	#include "emu68k.hpp"
	#include "system.hpp"
	#include "memspace.hpp"
	#include "hooks.hpp"
	#include "vinput.hpp"

//---------------------------------------------------

bool	freerun = false;
int		steps = 0;

int				cpu_type, cpu_trap_charin, cpu_trap_charout;
unsigned int	cpu_fcodes;

u_int8_t	*addrspace_ram;
u_int32_t	ram_start = RAM_START_DEFAULT;
u_int32_t	ram_end = RAM_END_DEFAULT;
u_int8_t	*addrspace_rom;
u_int32_t	rom_start = ROM_START_DEFAULT;
u_int32_t	rom_end = ROM_END_DEFAULT;

u_int32_t	uart_data = UART_DATA_DEFAULT;
u_int32_t	uart_status = UART_STATUS_DEFAULT;

Hooks		*emu68k_hooks;

//---------------------------------------------------

void init_ui(){
	GtkWidget *tmp;
	char tmpstr[12];

	tmp = GTK_WIDGET(gtk_builder_get_object(builder, "RAMstart"));
	sprintf(tmpstr, "%08X", RAM_START_DEFAULT);
	gtk_entry_set_text(GTK_ENTRY(tmp), tmpstr);
	tmp = GTK_WIDGET(gtk_builder_get_object(builder, "RAMstart1"));
	sprintf(tmpstr, "%08X", RAM_END_DEFAULT);
	gtk_entry_set_text(GTK_ENTRY(tmp), tmpstr);
	tmp = GTK_WIDGET(gtk_builder_get_object(builder, "ROMstart"));
	sprintf(tmpstr, "%08X", ROM_START_DEFAULT);
	gtk_entry_set_text(GTK_ENTRY(tmp), tmpstr);
	tmp = GTK_WIDGET(gtk_builder_get_object(builder, "ROMstart1"));
	sprintf(tmpstr, "%08X", ROM_END_DEFAULT);
	gtk_entry_set_text(GTK_ENTRY(tmp), tmpstr);
	tmp = GTK_WIDGET(gtk_builder_get_object(builder, "UARTdata"));
	sprintf(tmpstr, "%08X", UART_DATA_DEFAULT);
	gtk_entry_set_text(GTK_ENTRY(tmp), tmpstr);
	tmp = GTK_WIDGET(gtk_builder_get_object(builder, "UARTstatus"));
	sprintf(tmpstr, "%08X", UART_STATUS_DEFAULT);
	gtk_entry_set_text(GTK_ENTRY(tmp), tmpstr);

	addrspace_ram = (u_int8_t *)malloc(ram_end - ram_start);
	if (addrspace_ram == NULL) {
		printf("[!] Could not allocate memory for ram\n");
		exit(-1);
	} else {
		MemSpace::clear_addrspace_ram();
	}

	addrspace_rom = (u_int8_t *)malloc(rom_end - rom_start);
	if (addrspace_rom == NULL) {
		printf("[!] Could not allocate memory for rom\n");
		exit(-1);
	} else {
		MemSpace::clear_addrspace_rom();
	}

	GtkComboBox *cpuval = GTK_COMBO_BOX(gtk_builder_get_object(builder, "CPUselect"));
	cpu_type = M68K_CPU_TYPE_DEFAULT;
	switch(cpu_type) {
		case M68K_CPU_TYPE_68000:
			gtk_combo_box_set_active(cpuval, 0);
			break;
		case M68K_CPU_TYPE_68010:
			gtk_combo_box_set_active(cpuval, 1);
			break;
		case M68K_CPU_TYPE_68020:
			gtk_combo_box_set_active(cpuval, 2);
			break;
		case M68K_CPU_TYPE_68030:
			gtk_combo_box_set_active(cpuval, 3);
			break;
	}

	memView_address = ram_start;	// Set the memory window address to the start of RAM
	memViewUpdate(memViewBuffer);	// Update the memory window

	m68k_init();
	emu68k_hooks = new Hooks();		// Install emulation hooks
	m68k_set_cpu_type(cpu_type);
	m68k_pulse_reset();
	update_ui_regs();
}

void update_sysconfig(){
	GtkEntry *tmp;
	char tmpstr[12];
	bool modified = false;

	tmp = GTK_ENTRY(gtk_builder_get_object(builder, "RAMstart"));
	sprintf(tmpstr, "%08X", ram_start);
	if (strcmp(gtk_entry_get_text(tmp), tmpstr)) {
		ram_start = strtoul(gtk_entry_get_text(tmp), NULL, 16);
		modified = true;
		memView_address = ram_start;	// Reset the memory window to the start of the new RAM block
		memViewUpdate(memViewBuffer);	// Update the memory window
	}

	tmp = GTK_ENTRY(gtk_builder_get_object(builder, "RAMstart1"));
	sprintf(tmpstr, "%08X", ram_end);
	if (strcmp(gtk_entry_get_text(tmp), tmpstr)) {
		ram_end = strtoul(gtk_entry_get_text(tmp), NULL, 16);
		modified = true;
	}

	tmp = GTK_ENTRY(gtk_builder_get_object(builder, "ROMstart"));
	sprintf(tmpstr, "%08X", rom_start);
	if (strcmp(gtk_entry_get_text(tmp), tmpstr)) {
		rom_start = strtoul(gtk_entry_get_text(tmp), NULL, 16);
		modified = true;
	}

	tmp = GTK_ENTRY(gtk_builder_get_object(builder, "ROMstart1"));
	sprintf(tmpstr, "%08X", rom_end);
	if (strcmp(gtk_entry_get_text(tmp), tmpstr)) {
		rom_end = strtoul(gtk_entry_get_text(tmp), NULL, 16);
		modified = true;
	}

	tmp = GTK_ENTRY(gtk_builder_get_object(builder, "UARTdata"));
	sprintf(tmpstr, "%08X", uart_data);
	if (strcmp(gtk_entry_get_text(tmp), tmpstr)) {
		uart_data = strtoul(gtk_entry_get_text(tmp), NULL, 16);
		modified = true;
	}

	tmp = GTK_ENTRY(gtk_builder_get_object(builder, "UARTstatus"));
	sprintf(tmpstr, "%08X", uart_status);
	if (strcmp(gtk_entry_get_text(tmp), tmpstr)) {
		uart_status = strtoul(gtk_entry_get_text(tmp), NULL, 16);
		modified = true;
	}

	if (modified) {
		free(addrspace_ram);
		addrspace_ram = (u_int8_t *)malloc(ram_end - ram_start);
		if (addrspace_ram == NULL) {
			printf("[!] Could not allocate memory for ram\n");
			exit(-1);
		} else {
			MemSpace::clear_addrspace_ram();
		}

		free(addrspace_rom);
		addrspace_rom = (u_int8_t *)malloc(rom_end - rom_start);
		if (addrspace_rom == NULL) {
			printf("[!] Could not allocate memory for rom\n");
			exit(-1);
		} else {
			MemSpace::clear_addrspace_rom();
		}

		gtk_widget_show(Dialog_mm_change);
		gtk_window_set_keep_above(GTK_WINDOW(Dialog_mm_change), true);
	}

	GtkComboBox *cpuval = GTK_COMBO_BOX(gtk_builder_get_object(builder, "CPUselect"));
	switch(gtk_combo_box_get_active(cpuval)) {
		case 0:
		default:
			cpu_type = M68K_CPU_TYPE_68000;
			break;
		case 1:
			cpu_type = M68K_CPU_TYPE_68010;
			break;
		case 2:
			cpu_type = M68K_CPU_TYPE_68020;
			break;
		case 3:
			cpu_type = M68K_CPU_TYPE_68030;
			break;
	}
	m68k_set_cpu_type(cpu_type);
}

void update_ui_regs() {
	GtkEntry *tmp;
	char tmpstr[12];

	UI_UPDATE_REG("entry1", M68K_REG_D0);
	UI_UPDATE_REG("entry2", M68K_REG_D1);
	UI_UPDATE_REG("entry3", M68K_REG_D2);
	UI_UPDATE_REG("entry4", M68K_REG_D3);
	UI_UPDATE_REG("entry5", M68K_REG_D4);
	UI_UPDATE_REG("entry6", M68K_REG_D5);
	UI_UPDATE_REG("entry7", M68K_REG_D6);
	UI_UPDATE_REG("entry8", M68K_REG_D7);

	UI_UPDATE_REG("entry9", M68K_REG_A0);
	UI_UPDATE_REG("entry10", M68K_REG_A1);
	UI_UPDATE_REG("entry11", M68K_REG_A2);
	UI_UPDATE_REG("entry12", M68K_REG_A3);
	UI_UPDATE_REG("entry13", M68K_REG_A4);
	UI_UPDATE_REG("entry14", M68K_REG_A5);
	UI_UPDATE_REG("entry15", M68K_REG_A6);
	UI_UPDATE_REG("entry16", M68K_REG_A7);

	UI_UPDATE_REG("entry17", M68K_REG_PC);
	UI_UPDATE_REG("entry18", M68K_REG_SR);
	UI_UPDATE_REG("entry19", M68K_REG_IR);

	memViewUpdate(memViewBuffer);
}

void load_rom(char *fname){
	if (fname != NULL) {
		printf("loading file: %s\n", fname);
		FILE *fp = fopen(fname, "rb");
		if (fp != NULL) {
			fseek(fp, 0, SEEK_END);
			int fsize = ftell(fp);
			if (fsize > rom_end - rom_start) {
				gtk_widget_show(GTK_WIDGET(gtk_builder_get_object(builder, "Dialog_size")));
				return;
			}
			printf("fsize: %d\n", fsize);
			fseek(fp, 0, 0);
			fread(addrspace_rom, sizeof(u_int8_t), fsize, fp);
			freerun = false;
			m68k_pulse_reset();
			update_ui_regs();
		}
	}
}

void system_tick(size_t ticks){
	if (freerun) {
		gtk_spinner_start(GTK_SPINNER(activeSpin));
		m68k_execute(ticks);
	} else {
		while (steps > 0) {
			m68k_execute(1);
			steps--;
		}
		gtk_spinner_stop(GTK_SPINNER(activeSpin));
		GtkLabel *tmp = GTK_LABEL(gtk_builder_get_object(builder, "currentInst"));
		static char buff[100];
		m68k_disassemble(buff, m68k_get_reg(NULL, M68K_REG_PC), cpu_type);
		gtk_label_set_text(GTK_LABEL(tmp), buff);
	}
}

void make_hex(char* buff, unsigned int pc, unsigned int length) {
	char *ptr = buff;

	for(; length>0; length -= 2) {
		sprintf(ptr, "%04x", cpu_read_word_dasm(pc));
		pc += 2;
		ptr += 4;
		if(length > 2) *ptr++ = ' ';
	}
}

//---------------------------------------------------
