//---------------------------------------------------
//
//	ReASM v0.0
//	NotArtyom
//	08/05/18
//
//---------------------------------------------------

//---------------------Includes----------------------

	#include "config.h"
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <ctype.h>
	#include <string.h>
	#include <gtk/gtk.h>
	#include <vte/vte.h>

extern bool ui_input_locked;
#define isAddr(txt)	txt[strspn(txt,  "0123456789abcdefABCDEF")]

//---------------------------------------------------

int ui_regentry_callback(GtkWidget *widget){
	char* txt = gtk_entry_get_text(GTK_ENTRY(widget));
	if(isAddr(txt)){
		gtk_entry_set_text(GTK_ENTRY(widget), "");
	}
	return false;
}

int ui_regentry_makevalid(GtkWidget *widget, gchar *new, gint len, gpointer pos){
	*new = toupper(*new);
}
