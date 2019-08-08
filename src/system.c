//---------------------------------------------------
//
//	ReASM v0.0
//	NotArtyom
//	08/05/18
//
//---------------------------------------------------

	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <ctype.h>
	#include <string.h>
	#include <gtk/gtk.h>
	#include <vte/vte.h>

	#include "musashi/m68k.h"
	#include "config.h"

#define 	RAM_START_DEFAULT	0x00100000
#define 	RAM_END_DEFAULT		0x007FFFFF

#define 	ROM_START_DEFAULT	0x00000000
#define 	ROM_END_DEFAULT		0x000FFFFF

#define		UART_DATA_DEFAULT		0x00EFFC07
#define		UART_STATUS_DEFAULT		0x00EFFC03

#define		M68K_CPU_TYPE_DEFAULT	M68K_CPU_TYPE_68020

//---------------------------------------------------

bool		freerun = false;
int			steps = 0;

int				cpu_type;
unsigned int	cpu_fcodes;
int				cpu_trap_charin, cpu_trap_charout;

uint8_t		*addrspace_ram;
uint32_t	ram_start = RAM_START_DEFAULT;
uint32_t	ram_end = RAM_END_DEFAULT;

uint8_t		*addrspace_rom;
uint32_t	rom_start = ROM_START_DEFAULT;
uint32_t	rom_end = ROM_END_DEFAULT;

uint32_t	uart_data = UART_DATA_DEFAULT;
uint32_t	uart_status = UART_STATUS_DEFAULT;

//---------------------------------------------------

void init_ui(){
	GtkWidget *tmp;
	char tmpstr[12];

	tmp = gtk_builder_get_object(builder, "RAMstart");
	sprintf(&tmpstr, "%08X", RAM_START_DEFAULT);
	gtk_entry_set_text(tmp, &tmpstr);
	tmp = gtk_builder_get_object(builder, "RAMstart1");
	sprintf(&tmpstr, "%08X", RAM_END_DEFAULT);
	gtk_entry_set_text(tmp, &tmpstr);
	tmp = gtk_builder_get_object(builder, "ROMstart");
	sprintf(&tmpstr, "%08X", ROM_START_DEFAULT);
	gtk_entry_set_text(tmp, &tmpstr);
	tmp = gtk_builder_get_object(builder, "ROMstart1");
	sprintf(&tmpstr, "%08X", ROM_END_DEFAULT);
	gtk_entry_set_text(tmp, &tmpstr);
	tmp = gtk_builder_get_object(builder, "UARTdata");
	sprintf(&tmpstr, "%08X", UART_DATA_DEFAULT);
	gtk_entry_set_text(tmp, &tmpstr);
	tmp = gtk_builder_get_object(builder, "UARTstatus");
	sprintf(&tmpstr, "%08X", UART_STATUS_DEFAULT);
	gtk_entry_set_text(tmp, &tmpstr);

	addrspace_ram = (uint8_t *)malloc(ram_end - ram_start);
	if (addrspace_ram == NULL) {
		printf("[!] Could not allocate memory for ram\n");
		exit(-1);
	}

	addrspace_rom = (uint8_t *)malloc(rom_end - rom_start);
	if (addrspace_rom == NULL) {
		printf("[!] Could not allocate memory for rom\n");
		exit(-1);
	}

	tmp = gtk_builder_get_object(builder, "CPUselect");
	cpu_type = M68K_CPU_TYPE_DEFAULT;
	switch(cpu_type) {
		case M68K_CPU_TYPE_68000:
			gtk_combo_box_set_active(tmp, 0);
			break;
		case M68K_CPU_TYPE_68010:
			gtk_combo_box_set_active(tmp, 1);
			break;
		case M68K_CPU_TYPE_68020:
			gtk_combo_box_set_active(tmp, 2);
			break;
		case M68K_CPU_TYPE_68030:
			gtk_combo_box_set_active(tmp, 3);
			break;
	}

	m68k_init();
	m68k_set_cpu_type(cpu_type);
	m68k_pulse_reset();
	update_ui_regs();
}

void update_sysconfig(){
	GtkWidget *tmp;
	char tmpstr[12];
	bool modified = false;

	tmp = gtk_builder_get_object(builder, "RAMstart");
	sprintf(&tmpstr, "%08X", ram_start);
	if (strcmp(gtk_entry_get_text(tmp), &tmpstr)) {
		ram_start = strtoul(gtk_entry_get_text(tmp), NULL, 16);
		modified = true;
	}

	tmp = gtk_builder_get_object(builder, "RAMstart1");
	sprintf(&tmpstr, "%08X", ram_end);
	if (strcmp(gtk_entry_get_text(tmp), &tmpstr)) {
		ram_end = strtoul(gtk_entry_get_text(tmp), NULL, 16);
		modified = true;
	}

	tmp = gtk_builder_get_object(builder, "ROMstart");
	sprintf(&tmpstr, "%08X", rom_start);
	if (strcmp(gtk_entry_get_text(tmp), &tmpstr)) {
		rom_start = strtoul(gtk_entry_get_text(tmp), NULL, 16);
		modified = true;
	}

	tmp = gtk_builder_get_object(builder, "ROMstart1");
	sprintf(&tmpstr, "%08X", rom_end);
	if (strcmp(gtk_entry_get_text(tmp), &tmpstr)) {
		rom_end = strtoul(gtk_entry_get_text(tmp), NULL, 16);
		modified = true;
	}

	tmp = gtk_builder_get_object(builder, "UARTdata");
	sprintf(&tmpstr, "%08X", uart_data);
	if (strcmp(gtk_entry_get_text(tmp), &tmpstr)) {
		uart_data = strtoul(gtk_entry_get_text(tmp), NULL, 16);
		modified = true;
	}

	tmp = gtk_builder_get_object(builder, "UARTstatus");
	sprintf(&tmpstr, "%08X", uart_status);
	if (strcmp(gtk_entry_get_text(tmp), &tmpstr)) {
		uart_status = strtoul(gtk_entry_get_text(tmp), NULL, 16);
		modified = true;
	}

	if (modified) {
		free(addrspace_ram);
		addrspace_ram = (uint8_t *)malloc(ram_end - ram_start);
		if (addrspace_ram == NULL) {
			printf("[!] Could not allocate memory for ram\n");
			exit(-1);
		}

		free(addrspace_rom);
		addrspace_rom = (uint8_t *)malloc(rom_end - rom_start);
		if (addrspace_rom == NULL) {
			printf("[!] Could not allocate memory for rom\n");
			exit(-1);
		}

		gtk_widget_show(Dialog_mm_change);
		gtk_window_set_keep_above(Dialog_mm_change, true);
	}

	tmp = gtk_builder_get_object(builder, "CPUselect");
	switch(gtk_combo_box_get_active(tmp)) {
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

#define 	UI_UPDATE_REG(a, b) 						\
	tmp = gtk_builder_get_object(builder, a);			\
	sprintf(&tmpstr, "%08X", m68k_get_reg(NULL, b));	\
	gtk_entry_set_text(tmp, &tmpstr);

void update_ui_regs() {
	GtkWidget *tmp;
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
}

void load_rom(char *fname){
	if (fname != NULL) {
		printf("loading file: %s\n", fname);
		FILE *fp = fopen(fname, "rb");
		if (fp != NULL) {
			fseek(fp, 0, SEEK_END);
			int fsize = ftell(fp);
			if (fsize > rom_end - rom_start) {
				gtk_widget_show(gtk_builder_get_object(builder, "Dialog_size"));
				return;
			}
			printf("fsize: %d\n", fsize);
			fseek(fp, 0, 0);
			fread(addrspace_rom, sizeof(uint8_t), fsize, fp);
			freerun = false;
			m68k_pulse_reset();
			update_ui_regs();
		}
	}
}

void system_tick(){
	if (freerun) {
		gtk_spinner_start(activeSpin);
		m68k_execute(10000);
	} else {
		while (steps > 0) {
			m68k_execute(1);
			steps--;
		}
		gtk_spinner_stop(activeSpin);
		GtkWidget *tmp = gtk_builder_get_object(builder, "currentInst");
		static char buff[100];
		m68k_disassemble(buff, m68k_get_reg(NULL, M68K_REG_PC), cpu_type);
		gtk_label_set_text(tmp, &buff);
	}
}

void cpu_instr_callback(){
	if (freerun == false)
		update_ui_regs();
}

void cpu_set_fc(unsigned int fc){
	cpu_fcodes = fc;
}

int cpu_irq_ack(int level) {
	return M68K_INT_ACK_SPURIOUS;
}

void make_hex(char* buff, unsigned int pc, unsigned int length)
{
	char* ptr = buff;

	for(;length>0;length -= 2)
	{
		sprintf(ptr, "%04x", cpu_read_word_dasm(pc));
		pc += 2;
		ptr += 4;
		if(length > 2)
			*ptr++ = ' ';
	}
}

//---------------------------------------------------

unsigned int  cpu_read_byte(unsigned int address){
	if (address >= ram_start && address <= ram_end) {
		return READ_BYTE(addrspace_ram, address);
	} else if (address >= rom_start && address <= rom_end) {
		return READ_BYTE(addrspace_rom, address);
	} else if (address == uart_data) {
		return uart_input_buff;
	} else if (address == uart_status) {
		if (uart_status_byte == 0b00000101) {
			uart_status_byte = 0b00000100;
			return 0b00000101;
		} else {
			return 0b00000100;
		}
	} else {
		return 0x00;
	}
}

unsigned int  cpu_read_word(unsigned int address){
	if (address >= ram_start && address <= ram_end) {
		return READ_WORD(addrspace_ram, address);
	} else if (address >= rom_start && address <= rom_end) {
		return READ_WORD(addrspace_rom, address);
	} else if (address == uart_data) {
		return uart_input_buff;
	} else if (address == uart_status) {
		if (uart_status_byte == 0b00000101) {
			uart_status_byte = 0b00000100;
			return 0b00000101;
		} else {
			return 0b00000100;
		}
	} else {
		return 0x00;
	}
}

unsigned int  cpu_read_long(unsigned int address){
	if (address >= ram_start && address <= ram_end) {
		return READ_LONG(addrspace_ram, address);
	} else if (address >= rom_start && address <= rom_end) {
		return READ_LONG(addrspace_rom, address);
	} else if (address == uart_data) {
		return uart_input_buff;
	} else if (address == uart_status) {
		if (uart_status_byte == 0b00000101) {
			uart_status_byte = 0b00000100;
			return 0b00000101;
		} else {
			return 0b00000100;
		}
	} else {
		return 0x00;
	}
}

unsigned int cpu_read_word_dasm(unsigned int address){
	if (address >= ram_start && address <= ram_end) {
		return READ_WORD(addrspace_ram, address);
	} else if (address >= rom_start && address <= rom_end) {
		return READ_WORD(addrspace_rom, address);
	} else if (address == uart_data) {
		return uart_input_buff;
	} else if (address == uart_status) {
		if (uart_status_byte == 0b00000101) {
			uart_status_byte = 0b00000100;
			return 0b00000101;
		} else {
			return 0b00000100;
		}
	} else {
		return 0x00;
	}
}

unsigned int cpu_read_long_dasm(unsigned int address){
	if (address >= ram_start && address <= ram_end) {
		return READ_LONG(addrspace_ram, address);
	} else if (address >= rom_start && address <= rom_end) {
		return READ_LONG(addrspace_rom, address);
	} else if (address == uart_data) {
		return uart_input_buff;
	} else if (address == uart_status) {
		if (uart_status_byte == 0b00000101) {
			uart_status_byte = 0b00000100;
			return 0b00000101;
		} else {
			return 0b00000100;
		}
	} else {
		return 0x00;
	}
}
/* Write to anywhere */
void cpu_write_byte(unsigned int address, unsigned int value){
	if (address >= ram_start && address <= ram_end) {
		WRITE_BYTE(addrspace_ram, address, value);
	} else if (address >= rom_start && address <= rom_end) {
		WRITE_BYTE(addrspace_rom, address, value);
	} else if (address == uart_data) {
		vte_terminal_feed(Terminal, &value, 1);
	} else if (address == uart_status) {
		return;
	} else {
		return;
	}
}

void cpu_write_word(unsigned int address, unsigned int value){
	if (address >= ram_start && address <= ram_end) {
		WRITE_WORD(addrspace_ram, address, value);
	} else if (address >= rom_start && address <= rom_end) {
		WRITE_WORD(addrspace_rom, address, value);
	} else if (address == uart_data) {
		vte_terminal_feed(Terminal, &value, 1);
	} else if (address == uart_status) {
		return;
	} else {
		return;
	}
}

void cpu_write_long(unsigned int address, unsigned int value){
	if (address >= ram_start && address <= ram_end) {
		WRITE_LONG(addrspace_ram, address, value);
	} else if (address >= rom_start && address <= rom_end) {
		WRITE_LONG(addrspace_rom, address, value);
	} else if (address == uart_data) {
		vte_terminal_feed(Terminal, &value, 1);
	} else if (address == uart_status) {
		return;
	} else {
		return;
	}
}
