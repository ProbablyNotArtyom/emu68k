/*####################################################################*/
//
//	emu68k v0.7
//	NotArtyom
//	27/03/21
//
/*####################################################################*/

#ifndef _VINPUT_HPP
#define _VINPUT_HPP

	#include <cstdlib>
	#include <gtk/gtk.h>

#define VTE_BUFFER_MAX_SIZE	1000

/*####################################################################*/

typedef struct {
	char 	buffer[VTE_BUFFER_MAX_SIZE];
	int 	head;
	int 	tail;
	size_t	items;
} fifo_t;

/*####################################################################*/

extern "C" int ui_regentry_callback(GtkWidget *widget);
extern "C" int ui_regentry_makevalid(GtkWidget *widget, gchar *val, gint len, gpointer pos);

char vte_fifo_get(fifo_t *fifo);
int vte_fifo_add(fifo_t *fifo, char val);
void *vte_putchar_buffered(void *);
void vte_handle_overflow(fifo_t *fifo);
void vte_putchar(unsigned int *val);

/*####################################################################*/

#endif
