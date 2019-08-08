//---------------------------------------------------
//
//	ReASM v0.0
//	NotArtyom
//	08/05/18
//
//---------------------------------------------------

//---------------------Includes----------------------

	#include "config.h"
	#include "musashi/m68k.h"
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <ctype.h>
	#include <string.h>
	#include <gtk/gtk.h>
	#include <vte/vte.h>

//------------------Function Protos------------------

GtkWidget 	*SYSconfig;
GtkWidget 	*Dialog_mm_change;
GtkWidget 	*Chooser;
GtkWidget 	*Terminal;
GtkWidget 	*ui_regentry_frame;
GtkWidget	*activeSpin;
GtkWidget	*fileChooser;
GtkWidget	*memViewBuffer;
GtkWidget	*memView;

GtkBuilder	*builder;

//------------------Var Declarations-----------------

#define ui_regentry_lock()		gtk_widget_set_sensitive(ui_regentry_frame, false)
#define ui_regentry_unlock()	gtk_widget_set_sensitive(ui_regentry_frame, true)
static bool doExit = false;
static bool running = false;
bool ui_input_locked = true;

//-----------------------Main------------------------

int main (int argc, char *argv[]) {
    GtkWidget       *window;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "GTK.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "Sim_main"));
    SYSconfig = GTK_WIDGET(gtk_builder_get_object(builder, "SYS_prefs"));
	Dialog_mm_change = GTK_WIDGET(gtk_builder_get_object(builder, "Dialog_mm_change"));
    Chooser = GTK_WIDGET(gtk_builder_get_object(builder, "Chooser"));
    Terminal = GTK_WIDGET(gtk_builder_get_object(builder, "Terminal"));
    ui_regentry_frame = GTK_WIDGET(gtk_builder_get_object(builder, "grid1"));
	activeSpin = GTK_WIDGET(gtk_builder_get_object(builder, "activeSpin"));
	fileChooser = GTK_WIDGET(gtk_builder_get_object(builder, "fileChooser"));
	memView = GTK_WIDGET(gtk_builder_get_object(builder, "memViewer"));
	memViewBuffer = gtk_builder_get_object(builder, "textbuffer1");

    gtk_builder_connect_signals(builder, NULL);
	init_ui();
	memViewUpdate(memViewBuffer);

    gtk_widget_show_all(window);
	vte_terminal_feed(VTE_TERMINAL(Terminal), "", 0);
	if (argc > 1) {
		load_rom(argv[1]);
	}

	while(doExit == false){
		gtk_main_iteration();
		system_tick();
	}

    return 0;
}

void on_Sim_destroy() {
    doExit = true;
}

void Window_show(GtkWidget *win, gpointer user_data) {
	gtk_widget_show(user_data);
}

void Window_hide(GtkWidget *win, gpointer user_data) {
	gtk_widget_hide(user_data);
}

void on_SYS_prefs_close() {
	gtk_widget_hide(SYSconfig);
}

void on_SYS_prefs_response() {

}

void on_PrefsApply_clicked() {
	update_sysconfig();
	gtk_widget_hide(SYSconfig);
}

void on_PrefsCancel_clicked() {
	gtk_widget_hide(SYSconfig);
}

void on_ChooseApply_clicked() {
	gtk_widget_hide(fileChooser);
	load_rom(gtk_file_chooser_get_filename(fileChooser));
}

void on_ChooseCancel_clicked() {
	gtk_widget_hide(fileChooser);
}

void on_Return_clicked() {

}

void on_Next_clicked() {

}

void on_Reset_clicked() {
	freerun = false;
	m68k_pulse_reset();
	update_ui_regs();
}

void on_Stop_clicked() {
	freerun = false;
	update_ui_regs();
}

void on_Pause_clicked() {
	freerun = false;
	update_ui_regs();
}

void on_Run_clicked() {
	freerun = true;
	update_ui_regs();
}

void on_StepCount_clicked() {
	freerun = false;
	GtkWidget *tmp = gtk_builder_get_object(builder, "spinbutton1");
	steps = gtk_spin_button_get_value_as_int(tmp);
}

void on_StepOver_clicked() {
	int tmp = m68k_get_reg(NULL, M68K_REG_PC);
	m68k_set_reg(M68K_REG_PC, tmp + 4);
	update_ui_regs();
}

void on_Step_clicked() {
	freerun = false;
	steps = 1;
	update_ui_regs();
}

unsigned int 	uart_input_buff = 0x00;
unsigned int	uart_status_byte = 0b00000100;
void on_Terminal_commit(VteTerminal *vteterminal, guchar *text, guint size, gpointer user_data) {
	uart_input_buff = *text;
	uart_status_byte = 0b00000101;
}

//---------------------------------------------------

unsigned int memView_address;

void memViewUpdate(GtkWidget *widget) {
	char memViewText[100];
	GtkTextIter start, end;
	gtk_text_buffer_get_start_iter(memViewBuffer, &start);
	gtk_text_buffer_get_end_iter(memViewBuffer, &end);
	gtk_text_buffer_delete(memViewBuffer, &start, &end);
	gtk_text_buffer_get_start_iter(memViewBuffer, &start);

	for(int i = 0; i < 32; i++) {
		format_mem_line(&memViewText, memView_address+(16*i));
		gtk_text_buffer_insert(widget, &start, &memViewText, -1);
	}
}

void format_mem_line(char *buff, unsigned int addr) {
	sprintf(buff, "0x%08X | ", addr);
	for(int i = 0; i < 16; i++){
		sprintf(buff+i*3 + 12, " %02X", cpu_read_byte(i));
	}
	buff[60] = '\n';
	buff[61] = '\0';
}

void open_memory_view() {
	gtk_widget_show(memView);
}

void memView_up() {
	if (memView_address < 0xFFFFFE00)
		memView_address = memView_address + 0x200;
	memViewUpdate(memViewBuffer);
}

void memView_down() {
	if (memView_address > 0x00)
		memView_address = memView_address - 0x200;
	memViewUpdate(memViewBuffer);
}
