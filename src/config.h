#ifndef CONFIG_HEADER
#define CONFIG_HEADER

	#include <gtk/gtk.h>
	#include <stdbool.h>

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

//------------------Configuration--------------------

#define			FNamesize 255
#define			MAXVars 255
#define			MAXLabels 255
#define			TKSize	255

//--------------------No Toutch----------------------

enum {

	ERR_TYPENOVAR,
	ERR_UNDEFTYPE,
	ERR_GENERIC,
	ERR_NOMEM,
	ERR_MAXVARS,
	ERR_UNDEFVAR
};

//------------------Function Protos------------------

extern bool 	freerun;
extern int		steps;
extern uint8_t	*addrspace_rom;
extern uint8_t	*addrspace_ram;

extern GtkWidget 	*SYSconfig;
extern GtkWidget 	*Dialog_mm_change;
extern GtkWidget 	*Chooser;
extern GtkWidget 	*Terminal;
extern GtkWidget 	*ui_regentry_frame;
extern GtkWidget	*activeSpin;
extern GtkWidget	*memViewBuffer;
extern GtkWidget	*memView;

extern GtkBuilder	*builder;

extern unsigned int 	uart_input_buff;
extern unsigned int		uart_status_byte;

void load_rom(char *fname);
//---------------------------------------------------

/* Read/write macros */
#define READ_BYTE(BASE, ADDR) (BASE)[ADDR]
#define READ_WORD(BASE, ADDR) (((BASE)[ADDR]<<8) |			\
							  (BASE)[(ADDR)+1])
#define READ_LONG(BASE, ADDR) (((BASE)[ADDR]<<24) |			\
							  ((BASE)[(ADDR)+1]<<16) |		\
							  ((BASE)[(ADDR)+2]<<8) |		\
							  (BASE)[(ADDR)+3])

#define WRITE_BYTE(BASE, ADDR, VAL) (BASE)[ADDR] = (VAL)&0xff
#define WRITE_WORD(BASE, ADDR, VAL) (BASE)[ADDR] = ((VAL)>>8) & 0xff;		\
									(BASE)[(ADDR)+1] = (VAL)&0xff
#define WRITE_LONG(BASE, ADDR, VAL) (BASE)[ADDR] = ((VAL)>>24) & 0xff;		\
									(BASE)[(ADDR)+1] = ((VAL)>>16)&0xff;	\
									(BASE)[(ADDR)+2] = ((VAL)>>8)&0xff;		\
									(BASE)[(ADDR)+3] = (VAL)&0xff

#endif
