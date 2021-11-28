//---------------------------------------------------
//
//	emu68k v0.7
//	NotArtyom
//	27/03/21
//
//---------------------------------------------------

#ifndef _EMU68K_HEADER
#define _EMU68K_HEADER


	#include <gtk/gtk.h>
	#include <vte/vte.h>
	#include <stdbool.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <glib.h>

//------------------Configuration--------------------

#define 	RAM_START_DEFAULT	0x00100000
#define 	RAM_END_DEFAULT		0x006FFFFF

#define 	ROM_START_DEFAULT	0x00000000
#define 	ROM_END_DEFAULT		0x000FFFFF

#define		UART_DATA_DEFAULT		0x00EFFC07
#define		UART_STATUS_DEFAULT		0x00EFFC03

#define		M68K_CPU_TYPE_DEFAULT	M68K_CPU_TYPE_68020

//------------------Function Protos------------------

extern bool 			freerun;
extern int				steps;
extern u_int8_t			*addrspace_rom;
extern u_int8_t			*addrspace_ram;

extern GtkWidget 		*SYSconfig;
extern GtkWidget 		*Dialog_mm_change;
extern GtkWidget 		*Chooser;
extern GtkWidget 		*Terminal;
extern GtkWidget 		*ui_regentry_frame;
extern GtkWidget		*activeSpin;
extern GtkTextBuffer	*memViewBuffer;
extern GtkWidget		*memView;
extern GtkBuilder		*builder;

extern unsigned int 	uart_input_buff;
extern unsigned int		uart_status_byte;

extern u_int32_t 		memView_address;

void load_rom(char *fname);

//---------------------------------------------------

static inline u_int8_t READ_BYTE(u_int8_t *p, size_t a) {
	return (p)[a];
}

static inline u_int16_t READ_WORD(u_int8_t *p, size_t a) {
	return (((u_int16_t)(p)[a] << 8) | (u_int16_t)(p)[(a) + 1]);
}

static inline u_int32_t READ_LONG(u_int8_t *p, size_t a) {
	return (((u_int32_t)(p)[a] << 24) |
			((u_int32_t)(p)[(a) + 1] << 16) |
			((u_int32_t)(p)[(a) + 2] << 8) |
			(u_int32_t)(p)[(a) + 3]);
}

static inline void WRITE_BYTE(u_int8_t *p, size_t a, u_int8_t v) {
	(p)[a] = (v) & 0xff;
}

static inline void WRITE_WORD(u_int8_t *p, size_t a, u_int16_t v) {
	(p)[a] = ((v) >> 8) & 0xff;
	(p)[(a) + 1] = (v) & 0xff;
}

static inline void WRITE_LONG(u_int8_t *p, size_t a, u_int32_t v) {
	(p)[a] = ((v) >> 24) & 0xff;
	(p)[(a) + 1] = ((v) >> 16) & 0xff;
	(p)[(a) + 2] = ((v) >> 8) & 0xff;
	(p)[(a) + 3] = (v) & 0xff;
}

//---------------------------------------------------

void system_tick(size_t ticks);

extern "C" void Window_show(GtkWidget *win, gpointer user_data);
extern "C" void Window_hide(GtkWidget *win, gpointer user_data);
void memViewUpdate(GtkTextBuffer *widget);
void format_mem_line(char *buff, unsigned int addr);

extern "C" void on_Sim_destroy();
extern "C" void on_SYS_prefs_close();
extern "C" void on_SYS_prefs_response();
extern "C" void on_PrefsApply_clicked();
extern "C" void on_PrefsCancel_clicked();
extern "C" void on_ChooseApply_clicked();
extern "C" void on_ChooseCancel_clicked();

extern "C" void on_Return_clicked();
extern "C" void on_Next_clicked();
extern "C" void on_Reset_clicked();
extern "C" void on_Stop_clicked();
extern "C" void on_Pause_clicked();
extern "C" void on_Run_clicked();
extern "C" void on_StepCount_changed();
extern "C" void on_Chardelay_changed();
extern "C" void on_StepOver_clicked();
extern "C" void on_Steponce_clicked();
extern "C" void on_Step_clicked();

extern "C" void on_Terminal_commit(VteTerminal *vteterminal, guchar *text, guint size, gpointer user_data);
extern "C" void open_memory_view();
extern "C" void memView_up();
extern "C" void memView_down();

//---------------------------------------------------

#endif
