/*####################################################################*/
//
//      emu68k v0.7
//      NotArtyom
//      27/03/21
//
/*####################################################################*/

#include <cstdio>
#include <cstdlib>
#include <cstdbool>
#include <cctype>
#include <cstring>
#include <string>
#include <iostream>
#include <libgen.h>
#include <gtk/gtk.h>
#include <vte/vte.h>
#include <pthread.h>

#include "musashi/m68k.h"
#include "emu68k.hpp"
#include "system.hpp"
#include "memspace.hpp"
#include "vinput.hpp"

/*####################################################################*/

GtkWidget *SYSconfig;
GtkWidget *Dialog_mm_change;
GtkWidget *Chooser;
GtkWidget *Terminal;
GtkWidget *ui_regentry_frame;
GtkWidget *activeSpin;
GtkWidget *fileChooser;
GtkTextBuffer *memViewBuffer;
GtkWidget *memView;
GtkTextView *memViewArea;

GtkBuilder *builder;

/*####################################################################*/

#define ui_regentry_lock()		gtk_widget_set_sensitive(ui_regentry_frame, false)
#define ui_regentry_unlock()	gtk_widget_set_sensitive(ui_regentry_frame, true)

/* Global flags set by passed args */
bool doExit = false;
bool running = false;
bool autorun = false;
int debug = 0;			// Sets the verbosity of the debug output

/* Stuff for handling UI interaction */
bool ui_input_locked = true;
int vte_char_delay = 0;

/* CPU execution Stuff */
int instructions_per_step = 1;

static std::string helptxt = {
	"Gtk frontend for the Musashi m68k CPU simulator\r\n						\
	Usage: emu68k [-hdr][-t tickscale] path_to_rom\r\n							\
	\r\n																		\
	    -h               shows this help text\r\n								\
	    -r               automatically reset & run the CPU\r\n	 				\
	                     requires a ROM to be passed from the shell\r\n			\
	    -d               enables debug mode\r\n									\
	    -t tickscale     set the number of cycles in each emulation tick\r\n	\
	                     this is effectively a simulation speed multiplier\r\n	\
	                     high tickscales increase performance by sacrificing IO bandwith\r\n"
};

/*####################################################################*/

int main(int argc, char *argv[]) {
	GtkWidget *window;
	int opt, tickscale = 100000;
	char *rom_filename = NULL;

	while ((opt = getopt(argc, argv, "rhdt:")) != -1) {
		switch (opt) {
			case 'r':
				autorun = true;
				break;
			case 'h':
				std::cerr << helptxt.c_str();
				exit(0);
				break;
			case 'd':
				if (!debug) std::cout << "Debug mode enabled\n";
				debug++;
				break;
			case 't':
				tickscale = strtoul(optarg, NULL, 10);
				printf("tickscale set to %d\n", tickscale);
				break;
			default:
				break;
		}
	}

	for (int index = optind; index < argc; index++) {
		DEBUG_PRINTF(1, "Parsed ROM file: %s\n", argv[index]);
		rom_filename = argv[index];
	}

	gtk_init(&argc, &argv);
	builder = gtk_builder_new();

	/* find the install location of the binary in this system */
	char bindir[400];
	ssize_t pathsize = readlink("/proc/self/exe", bindir, sizeof(bindir) - 1);
	char gladeptr[400];

	strcpy(gladeptr, dirname(bindir));
	strcat(gladeptr, "/GTK.glade");
	gtk_builder_add_from_file(builder, gladeptr, NULL);

	window = GTK_WIDGET(gtk_builder_get_object(builder, "Sim_main"));
	SYSconfig = GTK_WIDGET(gtk_builder_get_object(builder, "SYS_prefs"));
	Dialog_mm_change = GTK_WIDGET(gtk_builder_get_object(builder, "Dialog_mm_change"));
	Chooser = GTK_WIDGET(gtk_builder_get_object(builder, "Chooser"));
	Terminal = GTK_WIDGET(gtk_builder_get_object(builder, "Terminal"));
	ui_regentry_frame = GTK_WIDGET(gtk_builder_get_object(builder, "grid1"));
	activeSpin = GTK_WIDGET(gtk_builder_get_object(builder, "activeSpin"));
	fileChooser = GTK_WIDGET(gtk_builder_get_object(builder, "fileChooser"));
	
	
	memView = GTK_WIDGET(gtk_builder_get_object(builder, "memViewer"));
	memViewArea = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "memViewArea"));
	memViewBuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "textbuffer1"));
	gtk_text_view_set_monospace(memViewArea, true);
	
	gtk_builder_connect_signals(builder, NULL);
	init_ui();
	memViewUpdate(memViewBuffer);
	gtk_widget_show_all(window);
	vte_terminal_feed(VTE_TERMINAL(Terminal), "", 0);

	if (rom_filename != NULL) {
		load_rom(rom_filename);
		if (autorun) {
			freerun = false;
			m68k_pulse_reset();
			freerun = true;
			update_ui_regs();
		}
	}

	while (doExit == false) {
		gtk_main_iteration();
		system_tick(tickscale);
	}
	return 0;
}

/*####################################################################*/

extern "C" void on_Sim_destroy() {
	doExit = true;
}

extern "C" void Window_show(GtkWidget * win, gpointer user_data) {
	gtk_widget_show(GTK_WIDGET(user_data));
}

extern "C" void Window_hide(GtkWidget * win, gpointer user_data) {
	gtk_widget_hide(GTK_WIDGET(user_data));
}

extern "C" void on_SYS_prefs_close() {
	gtk_widget_hide(SYSconfig);
}

extern "C" void on_SYS_prefs_response() {

}

extern "C" void on_PrefsApply_clicked() {
	update_sysconfig();
	gtk_widget_hide(SYSconfig);
}

extern "C" void on_PrefsCancel_clicked() {
	gtk_widget_hide(SYSconfig);
}

extern "C" void on_ChooseApply_clicked() {
	gtk_widget_hide(fileChooser);
	load_rom(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileChooser)));
}

extern "C" void on_ChooseCancel_clicked() {
	gtk_widget_hide(fileChooser);
}

extern "C" void on_Return_clicked() {

}

extern "C" void on_Next_clicked() {

}

extern "C" void on_Reset_clicked() {
	freerun = false;
	m68k_pulse_reset();
	MemSpace::clear_addrspace_ram();
	update_ui_regs();
}

extern "C" void on_Stop_clicked() {
	freerun = false;
	update_ui_regs();
}

extern "C" void on_Pause_clicked() {
	freerun = false;
	update_ui_regs();
}

extern "C" void on_Run_clicked() {
	freerun = true;
	update_ui_regs();
}

extern "C" void on_StepCount_changed() {
	GtkSpinButton *tmp = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton1"));
	instructions_per_step = gtk_spin_button_get_value_as_int(tmp);
}

extern "C" void on_Chardelay_changed() {
	GtkSpinButton *tmp = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "VTE_Speed"));
	vte_char_delay = gtk_spin_button_get_value_as_int(tmp);
}

extern "C" void on_StepOver_clicked() {
	int tmp = m68k_get_reg(NULL, M68K_REG_PC);
	m68k_set_reg(M68K_REG_PC, tmp + 4);
	update_ui_regs();
}

extern "C" void on_Steponce_clicked() {
	freerun = false;
	steps = 1;
	update_ui_regs();
}

extern "C" void on_Step_clicked() {
	freerun = false;
	steps = instructions_per_step;
	update_ui_regs();
}

unsigned int uart_input_buff = 0x00;
unsigned int uart_status_byte = u_int(4);
extern "C" void on_Terminal_commit(VteTerminal * vteterminal, guchar * text, guint size, gpointer user_data) {
	uart_input_buff = *text;
	uart_status_byte = u_int(5);
}

/*####################################################################*/

u_int32_t memView_address;
void memViewUpdate(GtkTextBuffer * widget) {
	char memViewText[32 * 16];
	GtkTextIter start, end;

	gtk_text_buffer_get_start_iter(memViewBuffer, &start);
	gtk_text_buffer_get_end_iter(memViewBuffer, &end);
	gtk_text_buffer_delete(memViewBuffer, &start, &end);
	gtk_text_buffer_get_start_iter(memViewBuffer, &start);

	for (int i = 0; i < 32; i++) {
		format_mem_line(memViewText, memView_address + (16 * i));
		gtk_text_buffer_insert(widget, &start, memViewText, -1);
	}
}

void format_mem_line(char *buff, unsigned int addr) {
	sprintf(buff, "0x%08X | ", addr);
	for (int i = 0; i < 16; i++) sprintf(buff + i * 3 + 12, " %02X", cpu_read_byte(addr + i));
	buff[60] = '\n';
	buff[61] = '\0';
}

extern "C" void open_memory_view() {
	memViewUpdate(memViewBuffer);
	gtk_widget_show(memView);
}

extern "C" void memView_down() {
	if (memView_address < 0xFFFFFE00) memView_address = memView_address + 0x200;
	memViewUpdate(memViewBuffer);
}

extern "C" void memView_up() {
	if (memView_address > 0x00) memView_address = memView_address - 0x200;
	memViewUpdate(memViewBuffer);
}

/*####################################################################*/

	
	
	
	
	
	
	
	
	
	
