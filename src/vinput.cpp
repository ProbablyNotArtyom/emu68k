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
	#include <pthread.h>

	#include "musashi/m68k.h"
	#include "emu68k.hpp"
	#include "memspace.hpp"
	#include "system.hpp"
	#include "vinput.hpp"

//---------------------------------------------------

extern bool		doExit;
extern bool 	debug;
extern int		vte_char_delay;

fifo_t			vte_buffer;
int 			vte_buffer_count;
bool			vte_buffer_lock;
pthread_t 		vte_buffer_thread_id;
pthread_mutex_t lock_buffer;

//---------------------------------------------------

extern "C" int ui_regentry_callback(GtkWidget *widget) {
	const char *txt = gtk_entry_get_text(GTK_ENTRY(widget));

	/* Check to make sure the input is a valid address */
	if(strspn(txt, "0123456789abcdefABCDEF") < strlen(txt)) {
		update_ui_regs();		// Invalid value detected
		return false;			// Restore the previous UI state and ignore the change
	} else {
		return true;
	}
}

extern "C" int ui_regentry_makevalid(GtkWidget *widget, gchar *val, gint len, gpointer pos) {
	*val = toupper(*val);
	return 1;
}

//---------------------------------------------------

char vte_fifo_get(fifo_t *fifo) {
	char tmp;
	// Check for available data
	if (fifo->tail != fifo->head) {
		tmp = fifo->buffer[fifo->tail];
		fifo->tail++;
		if (fifo->tail == VTE_BUFFER_MAX_SIZE) fifo->tail = 0;
		if (!iscntrl(tmp) && debug) printf("vte fifo get: \'%c\', head %d, tail %d\n", tmp, fifo->head, fifo->tail);
	} else return char(NULL);
	return tmp;
}

int vte_fifo_add(fifo_t *fifo, char val) {
	if ((fifo->head+1 == fifo->tail) || ((fifo->head+1 == VTE_BUFFER_MAX_SIZE) && (fifo->tail == 0))) vte_handle_overflow(fifo);
	fifo->buffer[fifo->head] = val;
	fifo->head++;
	if (fifo->head == VTE_BUFFER_MAX_SIZE) fifo->head = 0;
	if (!iscntrl(val) && debug) printf("vte fifo add: \'%c\', head %d, tail %d\n", val, fifo->head, fifo->tail);
	return true;
}

void *vte_putchar_buffered(void *) {
	while (doExit == false) {
		pthread_mutex_lock(&lock_buffer);
		if (vte_buffer.head != vte_buffer.tail && !vte_buffer_lock) {
			char tmp = vte_fifo_get(&vte_buffer);
			vte_terminal_feed(VTE_TERMINAL(Terminal), &tmp, 1);
			usleep(vte_char_delay*100);
		}
		pthread_mutex_unlock(&lock_buffer);
	}
	pthread_join(vte_buffer_thread_id, (void**)PTHREAD_PROCESS_SHARED);
	return nullptr;
}

void vte_handle_overflow(fifo_t *fifo) {
	vte_buffer_lock = true;
	while ((fifo->head != fifo->tail && fifo->tail != 0)) {
		char tmp = vte_fifo_get(fifo);
		vte_terminal_feed(VTE_TERMINAL(Terminal), &tmp, 1);
	}
	vte_buffer_lock = false;
}

void vte_putchar(unsigned int *val) {
	// Create a new thread for buffering the output
	// Use the existing thread if the buffer is still being emptied
	if (vte_buffer_thread_id == pthread_t(NULL)) {
		vte_buffer_lock = false;
		vte_buffer.tail = 0;
		vte_buffer.head = 0;
		pthread_mutex_init(&lock_buffer, NULL);
		pthread_create(&vte_buffer_thread_id, NULL, vte_putchar_buffered, NULL);
	}
	vte_fifo_add(&vte_buffer, *val);
}
